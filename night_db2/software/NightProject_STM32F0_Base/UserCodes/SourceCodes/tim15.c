/**
  ******************************************************************************
  * @file    TIM15.c
  * @author  Nightmare
  * @version V1.0.0
  * @date    2014.5.6
  * @brief   
  ******************************************************************************
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "appconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t PrescalerValue = 0;
static uint32_t TimerPeriod = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void TIM15_General_Config(uint32_t freq)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM15);

  /* Enable the TIM15 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TimerPeriod = (freq == 0) ? 0 : (SystemCoreClock / freq) - 1;

  /* TIM15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15 , ENABLE);
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);
   
  /* TIM Interrupts enable */
  TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);

  /* TIM15 enable counter */
  TIM_Cmd(TIM15, ENABLE);
}

void TIM15_PWM_Config(uint32_t freq)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA, GPIOB and GPIOE Clocks enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  /* GPIOB Configuration: TIM15 Channel 1 alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_1);
	
	TIM_DeInit(TIM15);

	TimerPeriod = (freq == 0) ? 0 : (SystemCoreClock / freq) - 1;

  /* TIM15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15 , ENABLE);
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);
	
	/* Channel 1, 2 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM15, &TIM_OCInitStructure);
	TIM_OC2Init(TIM15, &TIM_OCInitStructure);
   
  /* TIM15 enable counter */
  TIM_Cmd(TIM15, ENABLE);
	
	/* TIM15 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM15, ENABLE);
}

void TIM15_PWM_SetDuty(uint8_t ch, uint32_t duty)
{
	uint16_t PulseVal = (uint16_t)((duty * TimerPeriod) / 1000);
	switch(ch)
	{
		case 1:
			TIM_SetCompare1(TIM15, PulseVal);
		break;
		
		case 2:
			TIM_SetCompare2(TIM15, PulseVal);
		break;
	}	
}

void TIM15_General_Disable(void)
{
	TIM_Cmd(TIM15, DISABLE);
	TIM_ITConfig(TIM15, TIM_IT_Update, DISABLE);	
}

__IO uint32_t Timer15_Cnt = 0;
__IO uint32_t UpDownFlag = 0;
void TIM15_IRQHandler(void)
{
/*
	if(Timer15_Cnt == 1000)
	{
		printf("Timer15 interrupt occured!\n");
		Timer15_Cnt = 0;
	}
	Timer15_Cnt++;
	*/
	
	//Up & down conter used to implement breath LED
	TIM1_PWM_SetDuty(2, Timer15_Cnt);
	
	if(UpDownFlag == 0)
	{		
		if(Timer15_Cnt == 1000)
		{
			UpDownFlag = 1;			
		}
		else
		{			
			Timer15_Cnt++;
		}		
	}
	else
	{
		if(Timer15_Cnt == 0)
		{
			UpDownFlag = 0;			
		}
		else
		{			
			Timer15_Cnt--;
		}	
	}
	TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
}
