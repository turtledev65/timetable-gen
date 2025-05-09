#include "timetable.hpp"

/**
 * Availability
 */
Availability::Availability(int days, int periods)
    : m_Days(days), m_PeriodsPerDay(periods)
{
  assert(days >= 1 && days <= 7);
  assert(periods >= 1 && periods <= 32);

  m_Buffer.resize(m_Days, 0);
}

void Availability::Set(int day, int period, bool val)
{
  assert(day >= 0 && day < m_Days);
  assert(period >= 0 && period < m_PeriodsPerDay);

  unsigned int mask = 1 << period;
  if (val) {
    m_Buffer[day] |= mask;
  } else {
    m_Buffer[day] &= ~mask;
  }
}

void Availability::SetDay(int day, bool val)
{
  assert(day >= 0 && day < m_Days);

  if (val) {
    m_Buffer[day] = (1 << m_PeriodsPerDay) - 1;
  } else {
    m_Buffer[day] = 0;
  }
}

void Availability::ToggleDay(int day)
{
  assert(day >= 0 && day < m_Days);

  int mask = (1 << m_PeriodsPerDay) - 1;
  m_Buffer[day] ^= mask;
}

void Availability::Toggle(int day, int period)
{
  assert(day >= 0 && day < m_Days);
  assert(period >= 0 && period < m_PeriodsPerDay);

  unsigned int mask = 1 << period;
  m_Buffer[day] ^= mask;
}

bool Availability::Get(int day, int period) const
{
  assert(day >= 0 && day < m_Days);
  assert(period >= 0 && period < m_PeriodsPerDay);

  unsigned int mask = 1 << period;
  return (m_Buffer[day] & mask) != 0;
}

uint32_t Availability::GetDay(int day) const
{
  assert(day >= 0 && day < m_Days);
  return m_Buffer[day];
}

void Availability::Print(std::ostream &stream) const
{
  for (int day = 0; day < m_Days; day++) {
    stream << "Day " << day << ": ";
    for (int period = 0; period < m_PeriodsPerDay; period++) {
      bool isAvailable = Get(day, period);
      stream << isAvailable << ' ';
    }
    stream << '\n';
  }
}

/**
 * Lesson
 */
Lesson::Lesson(std::shared_ptr<const Class>   classPtr,
               std::shared_ptr<const Teacher> teacherPtr,
               std::shared_ptr<const Subject> subjectPtr, int periodsPerWeek)
    : m_Class(std::move(classPtr)), m_Teacher(std::move(teacherPtr)),
      m_Subject(std::move(subjectPtr)), m_PeriodsPerWeek(periodsPerWeek)
{
  assert(m_PeriodsPerWeek >= 1);
}

/**
 * Timetable
 */
bool Timetable::Generate()
{
  assert(m_Config.periodsPerDay > 0);
  assert(m_Config.subjects.size() > 0);
  assert(m_Config.teachers.size() > 0);
  assert(m_Config.classes.size() > 0);
  assert(m_Config.lessons.size() > 0);

  int totalAvailableSlots =
      m_Config.classes.size() * m_Config.days * m_Config.periodsPerDay;
  int requiredSlots = 0;
  for (const auto &lesson : m_Config.lessons) {
    requiredSlots += lesson->GetPeriodsPerWeek();
  }

  if (requiredSlots > totalAvailableSlots) {
    std::cerr << "Too many lessons for available time slots.\n";
    return false;
  }

  m_Schedule.clear();

  // Create slots for each period of the lessons
  std::vector<LessonSlot> lessonSlots;
  for (const auto &lessonPtr : m_Config.lessons) {
    for (int i = 0; i < lessonPtr->GetPeriodsPerWeek(); i++) {
      lessonSlots.emplace_back(lessonPtr, i);
    }
  }

  return GenerateImpl(lessonSlots, 0);
}

bool Timetable::GenerateImpl(std::vector<LessonSlot> &lessonSlots, int currIdx)
{
  // All lessons have been scheduled
  if (currIdx >= lessonSlots.size()) {
    return true;
  }
  std::cout << "Recursing" << currIdx << std::endl;

  // Defined for ease of use
  const auto &lesson  = lessonSlots[currIdx].first;
  const auto &cls     = lesson->GetClass();
  const auto &teacher = lesson->GetTeacher();
  const auto &subject = lesson->GetSubject();

  for (int day = 0; day < m_Config.days; day++) {
    for (int period = 0; period < m_Config.periodsPerDay; period++) {
      if (!cls->GetAvailability().Get(day, period) ||
          !teacher->GetAvailability().Get(day, period) ||
          !subject->GetAvailability().Get(day, period)) {
        continue;
      }

      if (!IsSlotFree(day, period, cls, teacher)) {
        continue;
      }

      m_Schedule.push_back({day, period, lesson});

      if (GenerateImpl(lessonSlots, currIdx + 1)) {
        return true;
      }

      m_Schedule.pop_back(); // backtrack
    }
  }

  return false;
}

bool Timetable::IsSlotFree(int day, int period,
                           std::shared_ptr<const Class>   cls,
                           std::shared_ptr<const Teacher> teacher)
{
  for (const auto &scheduledLesson : m_Schedule) {
    if (scheduledLesson.day == day && scheduledLesson.period == period) {
      if (scheduledLesson.lesson->GetClass() == cls ||
          scheduledLesson.lesson->GetTeacher() == teacher) {
        return false;
      }
    }
  }

  return true;
}

void Timetable::PrintConfig(std::ostream &stream) const
{
  stream << "Timetable Configuration:\n";
  stream << "Name: " << m_Config.name << "\n"
         << "Days: " << m_Config.days << "\n"
         << "Periods per Day: " << m_Config.periodsPerDay << "\n\n";

  stream << "Subjects:\n";
  for (const auto &subject : m_Config.subjects) {
    stream << "  - " << subject.GetName() << "\n";
  }

  stream << "\nTeachers:\n";
  for (const auto &teacher : m_Config.teachers) {
    stream << "  - " << teacher.GetName() << "\n";
  }

  stream << "\nClasses:\n";
  for (const auto &cls : m_Config.classes) {
    stream << "  - " << cls.GetName() << "\n";
  }

  stream << "\nLessons:\n";
  int index = 1;
  for (const auto &lesson : m_Config.lessons) {
    stream << "  Lesson " << index++ << "\n";
  }
}

// void Timetable::PrintSchedule(std::ostream &stream) const
// {
//   if (m_Schedule.empty()) {
//     stream << "Schedule is empty\n";
//     return;
//   }

//   for (const auto &cls : m_Config.classes) {
//     stream << "\n=== Timetable for " << cls.GetName() << " ===\n";

//     for (int day = 0; day < m_Config.days; ++day) {
//       stream << "Day " << day + 1 << ":\n";

//       for (int period = 0; period < m_Config.periodsPerDay; ++period) {
//         bool found = false;
//         for (const auto &sl : m_Schedule) {
//           if (sl.day == day && sl.period == period &&
//               sl.lesson->GetClass()->GetName() == cls.GetName()) {

//             stream << "  Period " << period + 1 << ": "
//                    << sl.lesson->GetSubject()->GetName() << " ("
//                    << sl.lesson->GetTeacher()->GetName() << ")\n";
//             found = true;
//             break;
//           }
//         }

//         if (!found) {
//           stream << "  Period " << period + 1 << ": Free\n";
//         }
//       }
//     }
//   }

void Timetable::PrintSchedule(std::ostream &stream) const
{
  const int days        = m_Config.days;
  const int periods     = m_Config.periodsPerDay;
  const int columnWidth = 25;

  // Collect scheduled lessons per class
  std::map<std::string, std::vector<std::vector<std::string>>> classSchedules;

  for (const auto &cls : m_Config.classes) {
    // Initialize empty timetable for the class
    classSchedules[cls.GetName()] = std::vector<std::vector<std::string>>(
        periods, std::vector<std::string>(days, ""));
  }

  for (const auto &scheduled : m_Schedule) {
    const auto &cls     = scheduled.lesson->GetClass();
    const auto &teacher = scheduled.lesson->GetTeacher();
    const auto &subject = scheduled.lesson->GetSubject();

    std::string entry = subject->GetName() + " (" + teacher->GetName() + ")";
    classSchedules[cls->GetName()][scheduled.period][scheduled.day] = entry;
  }

  // Print timetable for each class
  for (const auto &[className, table] : classSchedules) {
    stream << "Timetable for: " << className << "\n\n";

    // Print day headers
    stream << std::left << std::setw(columnWidth) << "";
    for (int day = 0; day < days; ++day) {
      stream << std::left << std::setw(columnWidth)
             << ("Day " + std::to_string(day));
    }
    stream << "\n";

    // Print each period's row
    for (int period = 0; period < periods; period++) {
      stream << std::left << std::setw(columnWidth)
             << ("Period " + std::to_string(period));
      for (int day = 0; day < days; ++day) {
        const std::string &entry = table[period][day];
        stream << std::left << std::setw(columnWidth)
               << (entry.empty() ? "-" : entry);
      }
      stream << "\n";
    }

    stream << "\n\n";
  }
}