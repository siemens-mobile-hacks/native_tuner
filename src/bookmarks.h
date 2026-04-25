#pragma once

#include <swilib.h>

extern uint32_t BOOKMARKS[9];

void Bookmarks_Serialize();
void Bookmarks_Deserialize();

int Bookmarks_Set(int id, uint32_t freq);
int Bookmarks_Find(uint32_t freq);
int Bookmarks_GetNext(int current);
int Bookmarks_GetPrev(int current);
int Bookmarks_GetFirst();
int Bookmarks_GetLast();
