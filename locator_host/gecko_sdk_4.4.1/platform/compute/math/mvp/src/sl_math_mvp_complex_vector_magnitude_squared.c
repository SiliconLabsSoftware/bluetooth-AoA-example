/***************************************************************************//**
 * @file
 * @brief MVP Math vector complex magnitude squared functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "sl_mvp.h"
#include "sl_mvp_util.h"
#include "sl_mvp_program_area.h"
#include "sl_math_mvp_complex_vector_magnitude_squared.h"
#include "sl_common.h"
#include <math.h>

#define USE_MVP_PROGRAMBUILDER    0

sl_status_t sl_math_mvp_complex_vector_magnitude_squared_f16(const float16_t *input,
                                                             float16_t *output,
                                                             size_t num_elements)
{
  sli_mvp_datatype_t data_type_in, data_type_out;
  uint32_t len_matrix;
  uint32_t len_remainder;
  size_t ofs_remainder;
  uint32_t rows, cols;

  if (!input || !output || !num_elements) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (!sli_mvp_util_is_pointer_word_aligned(input)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  data_type_in = SLI_MVP_DATATYPE_COMPLEX_BINARY16;
  data_type_out = SLI_MVP_DATATYPE_BINARY16;
  len_matrix = num_elements;
  len_remainder = 0;

  if (len_matrix <= SLI_MVP_MAX_ROW_LENGTH) {
    rows = 1;
    cols = len_matrix;
  } else {
    while (sli_mvp_util_factorize_number(len_matrix, 1024U, &rows, &cols) != SL_STATUS_OK) {
      len_matrix--;
      len_remainder++;
    }
  }
  ofs_remainder = num_elements - len_remainder;

#if USE_MVP_PROGRAMBUILDER
  sl_status_t status;
  const int vector_x = SLI_MVP_ARRAY(0);
  const int vector_z = SLI_MVP_ARRAY(1);
  const int vector_x2 = SLI_MVP_ARRAY(2);
  const int vector_z2 = SLI_MVP_ARRAY(3);


  sli_mvp_program_context_t *p = sli_mvp_get_program_area_context();
  sli_mvp_pb_init_program(p);
  sli_mvp_pb_begin_program(p);

  sli_mvp_pb_config_matrix(p->p, vector_x, (void *)input, data_type_in, rows, cols, &status);
  sli_mvp_pb_config_matrix(p->p, vector_z, (void *)output, data_type_out, rows, cols, &status);

  sli_mvp_pb_begin_loop(p, rows, &status); {
    sli_mvp_pb_begin_loop(p, cols, &status); {
      sli_mvp_pb_compute(p,
                         SLI_MVP_OP(SQRMAGC2),
                         SLI_MVP_ALU_X(SLI_MVP_R0)
                         | SLI_MVP_ALU_Z(SLI_MVP_R1),
                         SLI_MVP_LOAD(0, SLI_MVP_R0, vector_x, SLI_MVP_INCRDIM_WIDTH),
                         SLI_MVP_STORE(SLI_MVP_R1, vector_z, SLI_MVP_INCRDIM_WIDTH),
                         &status);
    }
    sli_mvp_pb_end_loop(p);
    sli_mvp_pb_postloop_incr_dim(p, vector_x, SLI_MVP_INCRDIM_HEIGHT);
    sli_mvp_pb_postloop_incr_dim(p, vector_z, SLI_MVP_INCRDIM_HEIGHT);
  }
  sli_mvp_pb_end_loop(p);

  if (len_remainder > 0) {
    sli_mvp_pb_config_vector(p->p, vector_x2, (void *)&input[ofs_remainder * 2], data_type_in, len_remainder, &status);
    sli_mvp_pb_config_vector(p->p, vector_z2, (void *)&output[ofs_remainder], data_type_out, len_remainder, &status);

    sli_mvp_pb_begin_loop(p, len_remainder, &status); {
      sli_mvp_pb_compute(p,
        SLI_MVP_OP(SQRMAGC2),
        SLI_MVP_ALU_X(SLI_MVP_R0)
        | SLI_MVP_ALU_Z(SLI_MVP_R1),
        SLI_MVP_LOAD(0, SLI_MVP_R0, vector_x2, SLI_MVP_INCRDIM_WIDTH),
        SLI_MVP_STORE(SLI_MVP_R1, vector_z2, SLI_MVP_INCRDIM_WIDTH),
        &status);
    }
    sli_mvp_pb_end_loop(p);
  }

  // Check if any errors found during program generation.
  if (status != SL_STATUS_OK) {
    return status;
  }
  if ((status = sli_mvp_pb_execute_program(p)) != SL_STATUS_OK) {
    return status;
  }
#else

  sli_mvp_cmd_enable();

  // Program array controllers.
  // Matrix input
  MVP->ARRAY[0].ADDRCFG = (sli_mvp_addr_reg_t)input;
  MVP->ARRAY[0].DIM0CFG =
    data_type_in << _MVP_ARRAYDIM0CFG_BASETYPE_SHIFT;
  MVP->ARRAY[0].DIM1CFG =
    ((rows - 1) << _MVP_ARRAYDIM1CFG_SIZE_SHIFT) | (cols << _MVP_ARRAYDIM1CFG_STRIDE_SHIFT);
  MVP->ARRAY[0].DIM2CFG =
    ((cols - 1) << _MVP_ARRAYDIM2CFG_SIZE_SHIFT) | (1 << _MVP_ARRAYDIM2CFG_STRIDE_SHIFT);
  // Matrix output
  MVP->ARRAY[1].ADDRCFG = (sli_mvp_addr_reg_t)output;
  MVP->ARRAY[1].DIM0CFG =
    data_type_out << _MVP_ARRAYDIM0CFG_BASETYPE_SHIFT;
  MVP->ARRAY[1].DIM1CFG =
    ((rows - 1) << _MVP_ARRAYDIM1CFG_SIZE_SHIFT) | (cols << _MVP_ARRAYDIM1CFG_STRIDE_SHIFT);
  MVP->ARRAY[1].DIM2CFG =
    ((cols - 1) << _MVP_ARRAYDIM2CFG_SIZE_SHIFT) | (1 << _MVP_ARRAYDIM2CFG_STRIDE_SHIFT);

  // Handle the first part of the input as a matrix.
  // Program loop controllers.
  MVP->LOOP[1].RST = 0;
  MVP->LOOP[0].CFG = (rows - 1) << _MVP_LOOPCFG_NUMITERS_SHIFT;
  MVP->LOOP[1].CFG = ((cols - 1) << _MVP_LOOPCFG_NUMITERS_SHIFT)
                     | ((SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_HEIGHT)
                         | SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_HEIGHT))
                        << _MVP_LOOPCFG_ARRAY0INCRDIM0_SHIFT);

  // Program instruction.
  MVP->INSTR[0].CFG0 = SLI_MVP_ALU_X(SLI_MVP_R0)
                       | SLI_MVP_ALU_Z(SLI_MVP_R1);
  MVP->INSTR[0].CFG1 = SLI_MVP_LOAD(0, SLI_MVP_R0, SLI_MVP_ARRAY(0), SLI_MVP_INCRDIM_WIDTH)
                       | SLI_MVP_STORE(SLI_MVP_R1, SLI_MVP_ARRAY(1), SLI_MVP_INCRDIM_WIDTH);
  MVP->INSTR[0].CFG2 = (SLI_MVP_OP(SQRMAGC2) << _MVP_INSTRCFG2_ALUOP_SHIFT)
                       | MVP_INSTRCFG2_LOOP0BEGIN
                       | MVP_INSTRCFG2_LOOP0END
                       | MVP_INSTRCFG2_LOOP1BEGIN
                       | MVP_INSTRCFG2_LOOP1END;

  size_t last_instruction_idx = 0;

  if (len_remainder > 0) {
    // Handle the remainder.
    // Program array controllers.
    MVP->ARRAY[2].ADDRCFG = (sli_mvp_addr_reg_t)&input[ofs_remainder * 2];
    MVP->ARRAY[2].DIM0CFG =
      data_type_in << _MVP_ARRAYDIM0CFG_BASETYPE_SHIFT;
    MVP->ARRAY[2].DIM1CFG =
      0;
    MVP->ARRAY[2].DIM2CFG = MVP->ARRAY[4].DIM2CFG =
      ((len_remainder - 1) << _MVP_ARRAYDIM2CFG_SIZE_SHIFT) | (1 << _MVP_ARRAYDIM2CFG_STRIDE_SHIFT);
    MVP->ARRAY[3].ADDRCFG = (sli_mvp_addr_reg_t)&output[ofs_remainder];
    MVP->ARRAY[3].DIM0CFG =
      data_type_out << _MVP_ARRAYDIM0CFG_BASETYPE_SHIFT;
    MVP->ARRAY[3].DIM1CFG =
      0;
    MVP->ARRAY[3].DIM2CFG =
      ((len_remainder - 1) << _MVP_ARRAYDIM2CFG_SIZE_SHIFT) | (1 << _MVP_ARRAYDIM2CFG_STRIDE_SHIFT);

      // Program loop controller.
      MVP->LOOP[2].CFG = (len_remainder - 1) << _MVP_LOOPCFG_NUMITERS_SHIFT
                        | ((SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_HEIGHT)
                          | SLI_MVP_LOOP_INCRDIM(SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_HEIGHT))
                          << _MVP_LOOPCFG_ARRAY0INCRDIM0_SHIFT);

      // Program instruction.
      MVP->INSTR[1].CFG0 = SLI_MVP_ALU_X(SLI_MVP_R0)
                          | SLI_MVP_ALU_Z(SLI_MVP_R1);
      MVP->INSTR[1].CFG1 = SLI_MVP_LOAD(0, SLI_MVP_R0, SLI_MVP_ARRAY(2), SLI_MVP_INCRDIM_WIDTH)
                          | SLI_MVP_STORE(SLI_MVP_R1, SLI_MVP_ARRAY(3), SLI_MVP_INCRDIM_WIDTH);
      MVP->INSTR[1].CFG2 = (SLI_MVP_OP(SQRMAGC2) << _MVP_INSTRCFG2_ALUOP_SHIFT)
                          | MVP_INSTRCFG2_LOOP2BEGIN
                          | MVP_INSTRCFG2_LOOP2END;

    // Move the next instruction index
    last_instruction_idx += 1;
  }

  // Insert end-of-program at correct position
  MVP->INSTR[last_instruction_idx].CFG2 |= MVP_INSTRCFG2_ENDPROG;

  // Start program.
  MVP->CMD = MVP_CMD_INIT | MVP_CMD_START;
#endif // USE_MVP_PROGRAMBUILDER

  return sli_mvp_cmd_wait_for_completion();
}
