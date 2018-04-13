#define _CRT_SECURE_NO_DEPRECATE
#include "fileparser.h"
#include "database.h"
void printStudents(Database *db, size_t i)
{
	if (list_count(db->student_list) > i)
	{
		Student *stud = list_get(db->student_list, i);
		printf("Student: %s (%d)\n", stud->first_name, stud->student_number);
		printStudents(db->student_list, ++i);
	}
}
void main(void)
{
	FPList *hmm;
	if (!(hmm = parse_database_file("C:\\Users\\Afonso\\Dropbox\\Internship Documents\\Course project sample ICT\\CAL - C Programming Assignment\\Code\\Main_Assignment_Code\\db.txt")))
	{
		printf("Unable to parse file. File non-existant or system out of memory\n");
		return;
	}

	Database *db = database_init(hmm);

	
	Student **students = 0;
	printf("Current students:\n");
	for (int i = get_students(db, &students) - 1; i >= 0; i--)
	{
		printf("Student: %s\n", (*(students + i))->first_name);
	}
	Teacher **teach = 0;
	if (students)
	{
		printf("\nTeachers that have tought %s:\n", (*students)->first_name);
		for (int i = find_teachers_by_student(db, &teach, *students) - 1; i >= 0; i--)
		{
			printf("Teacher: %s\n", (*(teach + i))->first_name);
		}
		if (!teach)
			printf("<none>");
	}
	printf("\nRemoving student with id 123456\n");
	remove_student(db, 123456);
	printf("Current students:\n");
	for (int i = get_students(db, &students) - 1; i >= 0; i--)
	{
		printf("Student: %s\n", (*(students + i))->first_name);
	}

	Course **cs = 0;
	if (students)
	{
		printf("\nCourses that %s (%d) is enrolled in:\n", (*students)->first_name, (*students)->student_number);
		for (int i = find_courses_by_student(db, &cs, *students) - 1; i >= 0; i--)
		{
			printf("Course: %s (%d)\n", (*cs)->course_name, (*cs)->course_number);
		}
		if (!cs)
			printf("<none>\n");
	}
	if (teach)
	{
		printf("\nCourses that teacher %s (%d) teaches:\n", (*teach)->first_name, (*teach)->teacher_number);
		for (int i = find_courses_by_teacher(db, &cs, *teach) - 1; i >= 0; i--)
		{
			printf("Course: %s (%d)\n", (*cs)->course_name, (*cs)->course_number);
		}
		if (!cs)
			printf("<none>\n");
	}

	if (students)
	{
		printf("\nTeachers that student %s (%d) has been taught by:\n", (*students)->first_name, (*students)->student_number);
		for (int i = find_teachers_by_student(db, &teach, *students) - 1; i >= 0; i--)
		{
			printf("Teacher: %s\n", (*(teach + i))->first_name);
		}
		if(!teach)
				printf("<none>\n");
	}
	while(1);
}
