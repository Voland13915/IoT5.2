#ifndef OVER_TEMP_GUARD_H
#define OVER_TEMP_GUARD_H

// Защита от перегрева с гистерезисом: срабатывает при temperature >= limit,
// отпускает при temperature <= release.
class OverTempGuard
{
public:
    OverTempGuard(float limitC, float releaseC);

    bool update(float temperatureC);
    bool isTripped() const;

private:
    float limitC;
    float releaseC;
    bool tripped;
};

#endif
