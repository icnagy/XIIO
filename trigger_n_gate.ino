void doTriggerFunction(bool todo){

  if (!todo){
    triggerLow;
    triggerState = 0;
  }

  if(todo){
    triggerHigh;
    triggerState = 1;

    triggerTime = millis();
  }

  doChange = 1;

}

void doGateFunction(bool todo){

  switch (todo){

    case 0:
    gateLow;
    gateState = 0;
    break;

    case 1:
    gateHigh;
    gateState = 1;
    gateHighTime = millis();
    break;
  }

}

