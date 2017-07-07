#ifndef SOUNDS_H
#define SOUNDS_H

#define G3  196
#define A3  220
#define B3  247
#define C4  262

#define BUZZER 7

class SoundEffects{
public:
  SoundEffects(){}
  void playClearMelody();
  void playCratePlaced();
  void playCrateRemoved();
};

void SoundEffects::playClearMelody(){
    int levelClearedMelody[] = {C4, G3, G3, A3, G3, 0, B3, C4};
    byte levelClearedDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
    byte len = sizeof(levelClearedMelody) / sizeof(levelClearedMelody[0]);
    for (byte note = 0; note < len; note++){
      int noteDuration = 600 / levelClearedDurations[note];
      tone(BUZZER, levelClearedMelody[note], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(BUZZER);
  }
}

void SoundEffects::playCratePlaced(){
  tone(BUZZER, G3, 100);
  tone(BUZZER, C4, 100);
  //delay(10);
  //noTone(BUZZER);
}

void SoundEffects::playCrateRemoved(){
  tone(BUZZER, C4, 100);
  tone(BUZZER, G3, 100);
  //delay(10);
  //noTone(BUZZER);
}

#endif
