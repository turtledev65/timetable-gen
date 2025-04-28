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

bool Availability::Get(int day, int period)
{
  assert(day >= 0 && day < m_Days);
  assert(period >= 0 && period < m_PeriodsPerDay);

  unsigned int mask = 1 << period;
  return (m_Buffer[day] & mask) != 0;
}

uint32_t Availability::GetDay(int day)
{
  assert(day >= 0 && day < m_Days);
  return m_Buffer[day];
}

void Availability::Print(std::ostream &stream)
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
void Timetable::Generate()
{
  assert(m_Config.periodsPerDay > 0);
  assert(m_Config.subjects.size() > 0);
  assert(m_Config.teachers.size() > 0);
  assert(m_Config.classes.size() > 0);
  assert(m_Config.lessons.size() > 0);

  GenerateImpl(0, 0);
}

void Timetable::GenerateImpl(int currDay, int currPeriod)
{
  if (currDay < 0 || currDay >= m_Config.days) {
    return;
  }
  if (currPeriod < 0) {
    return;
  }

  if (currPeriod >= m_Config.periodsPerDay) {
    GenerateImpl(currDay + 1, currPeriod);
  } else {
    GenerateImpl(currDay, currPeriod + 1);
  }
}

void Timetable::PrintConfig(std::ostream &stream)
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