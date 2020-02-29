// void glideDetermination(uint16_t noteToDac){

//   // get the note to which we are gliding
//   glideStop = noteToDac;

//   // if it's the same, return
//   if (glideNote == glideStop){
//     return;
//   }

//   // get direction of sweep
//   // down
//   if (glideNote > glideStop){
//     glideDirection = GLIDE_DIRECTION_DOWN;
//   }
//   // up
//   if (glideNote < glideStop){
//     glideDirection = GLIDE_DIRECTION_UP;
//   }

//   gliding = 1;

//   // calculate new gliding note
//   glideNote = glideNote - 1 + (glideDirection*2);

//   // refresh DAC  
//   writeDAC(glideNote);
// }
