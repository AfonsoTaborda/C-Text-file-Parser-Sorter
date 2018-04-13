#include "database.h"
#include <stddef.h>
#include <stdlib.h>

static size_t typed_list_to_ppp(List *list, void ***store, ENTITY_TYPE t)
{
	if (!(store && list))
		return 0;
	size_t c = list_count(list);
	void **courses = malloc(sizeof *courses * c);
	if (courses)
	{
		for (size_t i = 0; i < c; i++)
		{
			switch (t) {
			case COURSE:
				*((Course **)courses + i) = list_get(list, i);
				break;
			case STUDENT:
				*((Student **)courses + i) = list_get(list, i);
				break;
			case TEACHER:
				*((Teacher **)courses + i) = list_get(list, i);
				break;
			}
		}
		if(c)
			*store = courses;
		return c;
	}
	return 0;
}
static void *list_find_id(List *list, int32_t id)
{
	//Student, teacher and course is arranged with ids as the first 4 bytes in their respective structs.
	//So, we can just inspect the first 4 bytes (int *) and compare with the given id.
	for (int32_t i = list_count(list) - 1; i >= 0; i--)
	{
		int32_t *struct_id = list_get(list, i); //start address of struct
		if (struct_id && *struct_id == id) //compare first 4 bytes with id
			return struct_id; //return start address
	}
	return 0;
}
static size_t find(Database *db, void ***store, void* given, ENTITY_TYPE given_t, ENTITY_TYPE sought_t)
{
	List *tmp_store = new_list();
	if (!db)
		return 0;
	if (tmp_store)
	{
		switch (given_t) {
		case COURSE:
			if (sought_t == STUDENT)
			{
				for (int32_t i = list_count(db->enrollment_list) - 1; i >= 0; i--)
				{
					Enrollment *enroll = list_get(db->enrollment_list, i);
					if (enroll && enroll->course == given)
						list_add(tmp_store, enroll->student);
				}
			}
			else if(sought_t == TEACHER)
			{ } //not implemented
			break;
		case STUDENT:
			if (sought_t == TEACHER)
			{
				for (int32_t i = list_count(db->enrollment_list) - 1; i >= 0; i--)
				{
					Enrollment *enroll = list_get(db->enrollment_list, i);
					if (enroll && enroll->student == given)
					{
						for (int32_t x = list_count(db->assignment_list) - 1; x >= 0; x--)
						{
							Assignment *assign = list_get(db->assignment_list, x);
							if (assign && assign->course == enroll->course)
								list_add(tmp_store, assign->teacher);
						}
					}
				}
			}
			else if (sought_t == COURSE)
			{
				for (int32_t i = list_count(db->enrollment_list) - 1; i >= 0; i--)
				{
					Enrollment *enroll = list_get(db->enrollment_list, i);
					if (enroll && enroll->student == given)
						list_add(tmp_store, enroll->course);
				}
			}
			break;
		case TEACHER:
			if (sought_t == COURSE)
			{
				for (int32_t i = list_count(db->assignment_list) - 1; i >= 0; i--)
				{
					Assignment *assign = list_get(db->assignment_list, i);
					if (assign && assign->teacher == given)
						list_add(tmp_store, assign->course);
				}
			}
			else if(sought_t == STUDENT)
			{ } //not implemented
			break;
		}
		
		size_t r = typed_list_to_ppp(tmp_store, store, sought_t);
		free(tmp_store);
		return r;
	}
	return 0;
}
static void deduplicate_db(Database *db)
{
	for (int i = 0; i < 5; i++) //5 lists in FPList
	{
		List *list = *((List **)db + i);
		for (int32_t x = list_count(list) - 1; x > 0; x--)
		{
			for (int32_t j = x - 1; j >= 0; j--)
			{
				if (i < 3) //for student, teacher and course; the first 4 bytes is the primary key
				{
					int32_t *struct_id1 = list_get(list, x); //any 32bit type will do
					int32_t *struct_id2 = list_get(list, j);
					if (struct_id1 && struct_id2 && *struct_id1 == *struct_id2)
						list_remove(list, struct_id1); //duplicate; remove the struct pointed to by struct_id1
				}
				else
				{ 
					/*
					Assignment and enrollment lists:
					For assigment and enrollment the first 8 bytes is the composite primary key (course_number and teacher_number/student_number)
					We will assume that course_number is aligned at offset 4 in both structs. (teacher_number/student_number is guaranteed to be at 0)
					We should probably find the offset with offsetof(struct, member), but for now the following procedure works
					*/
					int64_t *struct_id1 = list_get(list, x); //any 64bit type will do
					int64_t *struct_id2 = list_get(list, j);
					if (struct_id1 && struct_id2 && *struct_id1 == *struct_id2)
						list_remove(list, struct_id1); //duplicate; remove the struct pointed to by struct_id1

				}
			}
		}
	}
}
static void link_assignments(Database *db)
{
	for (int32_t i = list_count(db->assignment_list) - 1; i >= 0; i--)
	{
		Assignment *assign = list_get(db->assignment_list, i);
		if (!((assign->teacher = find_teacher(db, assign->teacher_number)) &&
			(assign->course = find_course(db, assign->course_number))))
			list_remove(db->assignment_list, assign); //teacher and/or course not found so relation cannot exist.
	}
}
static void link_enrollments(Database *db)
{
	for (int32_t i = list_count(db->enrollment_list) - 1; i >= 0; i--)
	{
		Enrollment *enroll = list_get(db->enrollment_list, i);
		if (!((enroll->student = find_student(db, enroll->student_number)) &&
			(enroll->course = find_course(db, enroll->course_number))))
			list_remove(db->enrollment_list, enroll); //student and/or course not found so relation cannot exist.
	}
}

Database *database_init(FPList *file_parser_list)
{
	if (file_parser_list)
	{
		//dedup provided lists
		deduplicate_db(file_parser_list);

		//link enrollment and assignments
		link_assignments(file_parser_list);
		link_enrollments(file_parser_list);
	}
	return file_parser_list; //Database is compatible with FPList
}

Student *find_student(Database *db, int32_t student_number)
{
	return list_find_id(db->student_list, student_number);
}
Course *find_course(Database *db, int32_t course_number)
{
	return list_find_id(db->course_list, course_number);
}
Teacher *find_teacher(Database *db, int32_t teacher_number)
{
	return list_find_id(db->teacher_list, teacher_number);
}
size_t get_teachers(Database *db, Teacher ***store)
{
	return typed_list_to_ppp(db->teacher_list, store, TEACHER);
}
size_t get_students(Database *db, Student ***store)
{
	return typed_list_to_ppp(db->student_list, store, STUDENT);
}
size_t get_courses(Database *db, Course ***store)
{
	return typed_list_to_ppp(db->course_list, store, COURSE);
}
size_t find_courses_by_student(Database *db, Course ***store, Student *enrolled)
{
	return find(db, store, enrolled, STUDENT, COURSE);
}
size_t find_courses_by_teacher(Database *db, Course ***store, Teacher *teacher)
{
	return find(db, store, teacher, TEACHER, COURSE);
}
size_t find_students_by_course(Database *db, Student ***store, Course *course)
{
	return find(db, store, course, COURSE, STUDENT);
}
size_t find_teachers_by_student(Database *db, Teacher ***store, Student *student_of_teacher)
{
	return find(db, store, student_of_teacher, STUDENT, TEACHER);
}
int remove_student(Database *db, int32_t student_number)
{
	if (db)
		if (!list_remove(db->student_list, find_student(db, student_number))) //remove returns 0 on success, hence the !
		{
			link_enrollments(db); //student removed, check and remove enrollments
			return 1;
		}
	return 0;
}
