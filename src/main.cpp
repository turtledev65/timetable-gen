#include <iostream>
#include "timetable.hpp"

TimetableConfig getTestConfig()
{
  const int days          = 5;
  const int periodsPerDay = 6;

  // Helper: full availability
  auto FullAvailability = [days, periodsPerDay]() -> Availability {
    Availability a(days, periodsPerDay);
    for (int d = 0; d < days; ++d) {
      a.SetDay(d, true);
    }
    return a;
  };

  // Shared availability for simplicity
  Availability sharedAvailability = FullAvailability();

  // Subjects
  auto math    = std::make_shared<Subject>("Math", sharedAvailability);
  auto english = std::make_shared<Subject>("English", sharedAvailability);

  // Teachers
  auto alice = std::make_shared<Teacher>("Alice", sharedAvailability);
  auto bob   = std::make_shared<Teacher>("Bob", sharedAvailability);

  // Classes
  auto classA = std::make_shared<Class>("Class A", sharedAvailability);
  auto classB = std::make_shared<Class>("Class B", sharedAvailability);

  // Lessons
  Lesson lesson1(classA, alice, math, 2);
  Lesson lesson2(classB, bob, english, 2);

  // Timetable config
  TimetableConfig config;
  config.name          = "Test Timetable";
  config.periodsPerDay = periodsPerDay;

  // Subjects, Teachers, Classes (stored by value, dereference from shared_ptr)
  config.subjects = {*math, *english};
  config.teachers = {*alice, *bob};
  config.classes  = {*classA, *classB};

  // Lessons (store by value)
  config.lessons = {lesson1, lesson2};

  return config;
}

int main()
{
  Timetable table(getTestConfig());
  table.Generate();
  table.PrintConfig(std::cout);

  std::cin.get();
}
