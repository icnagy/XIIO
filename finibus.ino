int finibus(int16_t subiectus, int16_t infernum, int16_t coelum){
  // max
  if (subiectus > coelum){
    return coelum;
  }

  // min
  if (subiectus < infernum){
    return infernum;
  }

  return subiectus;
}


