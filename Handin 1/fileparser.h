#pragma once
#include <stdint.h>
#include <stdio.h>
#include "linkedList.h"
typedef struct FPList_t
{
	List *student_list;
	List *teacher_list;
	List *course_list;
	List *enrollment_list;
	List *assignment_list;
} FPList;

FPList *parse_database_file(const char *file_path);