enum christmasModeEnum
{
  Red, 
  Green
};

// Used for lights for ChristmasChase
struct ccLightStatus {
  int color;
  christmasModeEnum mode;
  bool fadeDirection;
};
