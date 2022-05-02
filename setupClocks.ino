void setupClocks(){
  // set up clocks  
  CCM_CCGR4 |= CCM_CCGR4_PWM2(CCM_CCGR_ON);
  
  //FLEXPWM2_MCTRL = 0; // make sure RUN flags are off
  FLEXPWM2_FCTRL0 = FLEXPWM_FCTRL0_FLVL(15); // logic high = fault
  FLEXPWM2_FSTS0 = 0x000F; // clear fault status
  FLEXPWM2_FFILT0 = 0;
  FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_CLDOK(15);

  setup_PWM();  // replaces setup for ICG, SH, MClock and ADC
  FLEXPWM2_MCTRL |= FLEXPWM_MCTRL_LDOK(15); // do counters initialize upon loading?
}
