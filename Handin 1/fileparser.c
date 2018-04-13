#define _CRT_SECURE_NO_DEPRECATE
#include "fileparser.h"
#include "database.h"
#include <stdlib.h>


FPList *parse_database_file(const char *file_path)
{
	FILE *fp;
	if (!(fp = fopen(file_path, "r")))
		return 0; //failed to open file

	FPList *list;

	if (!(list = (FPList *)malloc(sizeof *list)))
		return 0; //failed to allocate memory

	list->course_list = new_list();
	list->student_list = new_list();
	list->teacher_list = new_list();
	list->assignment_list = new_list();
	list->enrollment_list = new_list();
	
	if (!(list->course_list && list->student_list && list->teacher_list && list->assignment_list && list->enrollment_list))
	{
		free(list);
		fclose(fp);
		return 0;
	}

	char lineid = 0;
	while (fscanf(fp, "%c ", &lineid) != EOF)
	{
		switch (lineid)
		{
		case 's':
		case 'S':
		{
			char *student_name = (char*)malloc(sizeof(char) * 200);
			Student *student = (Student*)malloc(sizeof *student);
			if (student_name && student)
			{
				if (2 == fscanf(fp, "%d %199s", &(student->student_number), student_name))
				{
					student->first_name = student_name;
					list_add(list->student_list, student);
				}
				else
				{
					free(student);
					free(student_name);
				}
			}
		}
		break;
		case 't':
		case 'T':
		{
			char *teacher_name = (char*)malloc(sizeof(char) * 200);
			Teacher *teacher = (Teacher*)malloc(sizeof *teacher);
			if (teacher_name && teacher)
			{
				if (2 == fscanf(fp, "%d %199s", &(teacher->teacher_number), teacher_name))
				{
					teacher->first_name = teacher_name;
					list_add(list->teacher_list, teacher);
				}
				else
				{
					free(teacher);
					free(teacher_name);
				}
			}
		}
		break;
		case 'c':
		case 'C':
		{
			char *course_name = (char*)malloc(sizeof(char) * 200);
			Course *course = (Course*)malloc(sizeof *course);
			if (course_name && course)
			{
				if (3 == fscanf(fp, "%d %199s %d", &(course->course_number), course_name, &(course->semester_number)))
				{
					course->course_name = course_name;
					list_add(list->course_list, course);
				}
				else
				{
					free(course);
					free(course_name);
				}
			}
		}
		break;
		case 'e':
		case 'E':
		{
			Enrollment *enrol = (Enrollment*)malloc(sizeof *enrol);
			int32_t student_number;
			int32_t course_number;
			if (enrol)
			{
				if (2 == fscanf(fp, "%d %d", &student_number, &course_number))
				{
					enrol->student_number = student_number;
					enrol->course_number = course_number;
					list_add(list->enrollment_list, enrol);
				}
				else
					free(enrol);
			}
		}
		break;
		case 'a':
		case 'A':
		{
			Assignment *assign = (Assignment*)malloc(sizeof *assign);
			int32_t teacher_number;
			int32_t course_number;
			if (assign)
			{
				if (2 == fscanf(fp, "%d %d", &teacher_number, &course_number))
				{
					assign->course_number = course_number;
					assign->teacher_number = teacher_number;
					list_add(list->assignment_list, assign);
				}
				else
					free(assign);
			}
		}
		break;
		default:
			break;
		}
	}
	fclose(fp);
	return list;
}
