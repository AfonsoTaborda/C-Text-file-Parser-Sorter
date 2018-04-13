#pragma once
#include <stdint.h>
#include "linkedList.h"
#include "fileparser.h"
typedef struct Student_t
{
	int32_t student_number;
	char *first_name;
} Student;
typedef struct Teacher_t
{
	int32_t teacher_number;
	char *first_name;
} Teacher;
typedef struct Course_t
{
	int32_t course_number;
	char *course_name;
	int32_t semester_number;
} Course;
typedef struct Enrollment_t
{
	int32_t student_number;
	int32_t course_number;
	Student *student;
	Course *course;
} Enrollment;
typedef struct Assignment_t
{
	int32_t teacher_number;
	int32_t course_number;
	Teacher *teacher;
	Course *course;
} Assignment;

typedef FPList Database;

typedef enum {
	STUDENT,
	COURSE,
	TEACHER
} ENTITY_TYPE;

Database *database_init(FPList *file_parser_list);

Student *find_student(Database *db, int32_t student_number);
Course *find_course(Database *db, int32_t course_number);
Teacher *find_teacher(Database *db, int32_t teacher_number);
size_t get_teachers(Database *db, Teacher ***store);
size_t get_students(Database *db, Student ***store);
size_t get_courses(Database *db, Course ***store);
size_t find_courses_by_student(Database *db, Course ***store, Student *enrolled);
size_t find_courses_by_teacher(Database *db, Course ***store, Teacher *teacher);
size_t find_students_by_course(Database *db, Student ***store, Course *course);
size_t find_teachers_by_student(Database *db, Teacher ***store, Student *student_of_teacher);
int remove_student(Database *db, int32_t student_number);