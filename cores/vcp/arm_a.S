// SPDX-License-Identifier: Apache-2.0

/*******************************************************************************
  
*   Copyright (c) Telechips Inc.
  
*******************************************************************************/
/*****************************************************
 *
 * arm.S
 * Telechips TCC70xx SoCs Cortex-A7 Single Processor
 *
 * History
 * -------------
 * Created by : SangWon, Lee  2017/1/31
 *
 *****************************************************/

.global ARM_DSB


ARM_DSB:
        DSB
        ISB
        BX      LR
        
	.end
