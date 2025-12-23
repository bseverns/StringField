#include <unity.h>

#include "gesture_engine.h"

void test_pluck_then_bow_transition() {
  GestureParams params;
  GestureEngine engine(params);

  TEST_ASSERT_EQUAL(Gesture::Idle, engine.update({0.0f, 0}));
  TEST_ASSERT_EQUAL(Gesture::Pluck, engine.update({0.7f, 100000}));
  TEST_ASSERT_EQUAL(Gesture::Bow, engine.update({0.6f, 130000}));
}

void test_scrape_vs_pluck_spacing() {
  GestureParams params;
  params.mute_window_us = 10000;  // shorten so quick releases are not muted here
  GestureEngine engine(params);

  TEST_ASSERT_EQUAL(Gesture::Idle, engine.update({0.05f, 0}));
  TEST_ASSERT_EQUAL(Gesture::Pluck, engine.update({0.7f, 100000}));
  TEST_ASSERT_EQUAL(Gesture::Idle, engine.update({0.2f, 120000}));  // release, but not muted
  TEST_ASSERT_EQUAL(Gesture::Scrape, engine.update({0.65f, 135000}));
}

void test_quick_release_marks_mute() {
  GestureParams params;
  GestureEngine engine(params);

  TEST_ASSERT_EQUAL(Gesture::Pluck, engine.update({0.55f, 100000}));
  TEST_ASSERT_EQUAL(Gesture::Muted, engine.update({0.1f, 120000}));
}

void test_vibrato_after_wobbles() {
  GestureParams params;
  GestureEngine engine(params);

  TEST_ASSERT_EQUAL(Gesture::Pluck, engine.update({0.7f, 100000}));
  TEST_ASSERT_EQUAL(Gesture::Bow, engine.update({0.65f, 110000}));
  TEST_ASSERT_EQUAL(Gesture::Bow, engine.update({0.5f, 115000}));
  TEST_ASSERT_EQUAL(Gesture::Bow, engine.update({0.8f, 120000}));
  TEST_ASSERT_EQUAL(Gesture::Bow, engine.update({0.45f, 125000}));
  TEST_ASSERT_EQUAL(Gesture::Vibrato, engine.update({0.78f, 130000}));
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_pluck_then_bow_transition);
  RUN_TEST(test_scrape_vs_pluck_spacing);
  RUN_TEST(test_quick_release_marks_mute);
  RUN_TEST(test_vibrato_after_wobbles);
  return UNITY_END();
}

