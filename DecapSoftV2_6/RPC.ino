bool M4TaskCompleted(){
  *M4work_pntr = false;
  return true;
}

//Properly set the flags once the init is done
bool initDone(){
  *M4work_pntr = false;
  *isInit_pntr = true;
  *capHeld_pntr = false;
  motorOFF;
  mvt_out;
  Serial.print("initDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  return true;
}

bool decapDone(){
  *M4work_pntr = false;
  *capHeld_pntr = true;
  motorOFF;
  mvt_out;
  relayOFF;
  Serial.print("decapDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  return true;
}

bool recapDone(){
  *M4work_pntr = false;
  *capHeld_pntr = false;
  motorOFF;
  mvt_out;
  relayOFF;
  Serial.print("recapDone, took ");Serial.print(millis()-task_start_time);Serial.println("ms");
  return true;
}
