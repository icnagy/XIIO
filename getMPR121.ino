int getMPR121() {

  uint16_t MPR121_DATA = cap.touched();

  /*
    (00)(01)(02)(03)
    (08)(09)(10)(11) <- notePlates
    (04)(05)(06)(07) <- notePlates
  */

  bitWrite(plates, 0, bitRead(MPR121_DATA, 8));
  bitWrite(plates, 1, bitRead(MPR121_DATA, 7));
  bitWrite(plates, 2, bitRead(MPR121_DATA, 5));
  bitWrite(plates, 3, bitRead(MPR121_DATA, 3));

  bitWrite(plates, 8, bitRead(MPR121_DATA, 10));
  bitWrite(plates, 9, bitRead(MPR121_DATA, 11));
  bitWrite(plates, 10, bitRead(MPR121_DATA, 4));
  bitWrite(plates, 11, bitRead(MPR121_DATA, 0));

  bitWrite(plates, 4, bitRead(MPR121_DATA, 6));
  bitWrite(plates, 5, bitRead(MPR121_DATA, 9));
  bitWrite(plates, 6, bitRead(MPR121_DATA, 1));
  bitWrite(plates, 7, bitRead(MPR121_DATA, 2));

  notePlates = plates >> 4;

  switch (page) {

    case keyboard_page:
      octavePlates();
      getNote();
      break;

    case settings_page:

      if (plates != platesLast) {

        if (plates > 3) {

          // get the last touched plate
          for (int i = 0; i < 12; i++) {
            if (bitRead(plates & 0xFFC, i) == 1 && bitRead(platesLast & 0xFFC, i) == 0) {
              activePlate = i + 1;
              doChange = 1;
            }
          }

          // do editing with octave keys
          uint8_t variPlate = 0;
          for (int i = 0; i < 2; i++) {
            if (bitRead(plates & 0x3, i) == 1 && bitRead(platesLast & 0x3, i) == 0) {
              variPlate = i + 1;
            }
          }

          if (variPlate == 1) {
            edit(-1);
            doChange = 0;
          }
          if (variPlate == 2) {
            edit(+1);
            doChange = 0;
          }
        }

        // octave mode and action: no editing with octave keys
        if (plates <= 3) {
          if (plates & 0x3 < platesLast & 0x3) {
            activePlate = 0;
            doChange = 1;
          }
          for (int i = 0; i < 12; i++) {
            if (bitRead(plates & 0x3, i) == 1 && bitRead(platesLast & 0x3, i) == 0) {
              activePlate = i + 1;
              doChange = 1;
            }
          }
        }

        if (bitRead(plates, activePlate - 1) == 0 && bitRead(platesLast, activePlate - 1) == 1) {
          activePlate = 0;
          doChange = 1;
        }
      }

      break;
  }

  platesLast = plates;

}
