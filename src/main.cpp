#include <iostream>
#include "timetable.hpp"

TimetableConfig getTestConfig()
{
  const int days          = 5;
  const int periodsPerDay = 6;

  TimetableConfig config;
  config.name          = "Challenging Test Timetable";
  config.days          = days;
  config.periodsPerDay = periodsPerDay;

  // Helper: Full or Partial Availability
  auto FullAvailability = [&]() {
    Availability a(days, periodsPerDay);
    for (int d = 0; d < days; ++d)
      a.SetDay(d, true);
    return a;
  };

  auto PartialAvailability = [&](std::vector<int> availableDays) {
    Availability a(days, periodsPerDay);
    for (int d : availableDays)
      a.SetDay(d, true);
    return a;
  };

  // Subjects: 15 subjects
  std::vector<std::shared_ptr<Subject>> subjectPtrs;
  for (int i = 0; i < 15; ++i) {
    auto subj = std::make_shared<Subject>("Subject " + std::to_string(i + 1),
                                          FullAvailability());
    subjectPtrs.push_back(subj);
    config.subjects.push_back(*subj);
  }

  // Teachers: 8 teachers, some teach multiple subjects
  std::vector<std::shared_ptr<Teacher>> teacherPtrs;
  for (int i = 0; i < 8; ++i) {
    Availability avail =
        (i % 2 == 0) ? FullAvailability() : PartialAvailability({0, 1, 3});
    auto teacher =
        std::make_shared<Teacher>("Teacher " + std::to_string(i + 1), avail);
    teacherPtrs.push_back(teacher);
    config.teachers.push_back(*teacher);
  }

  // Classes: 6 total, some with full availability, some partial
  std::vector<std::shared_ptr<Class>> classPtrs;
  for (int i = 0; i < 6; ++i) {
    Availability avail =
        (i < 4) ? FullAvailability() : PartialAvailability({1, 2, 4});
    auto cls = std::make_shared<Class>("Class " + std::to_string(i + 1), avail);
    classPtrs.push_back(cls);
    config.classes.push_back(*cls);
  }

  // Lessons: Create 60 lessons, reusing some teachers and mixing subjects
  for (int i = 0; i < 60; ++i) {
    auto cls     = classPtrs[i % classPtrs.size()];
    auto subject = subjectPtrs[i % subjectPtrs.size()];

    // Reuse teachers: some teach multiple subjects
    auto teacher = teacherPtrs[(i / 2) % teacherPtrs.size()];

    // Periods: vary between 2 and 5
    int periods = 2 + (i % 4);

    auto lesson = std::make_shared<Lesson>(cls, teacher, subject, periods);
    config.lessons.push_back(lesson);
  }

  return config;
}

int main()
{
  Timetable table(getTestConfig());
  table.PrintConfig(std::cout);

  if (!table.Generate()) {
    std::cout << "Failed to generate";
  }
  // table.PrintSchedule(std::cout);

  // std::cin.get();
}