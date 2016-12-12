/*Training Project I: Corner Stitching*/
/* 2016/12 Shih-Ting Lin */
#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <stdlib.h> 
#include <crtdbg.h>  
using namespace std;

#define _CRTDBG_MAP_ALLOC 

#ifdef _DEBUG 
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif 
#endif  // _DEBUG 



int newSize = 0;
int delSize = 0;

int layout_width;
int layout_height;
/*
B-------C
|       |
|       |
A-------D
*/
int lay[8] = {0};

typedef struct tile {
	int point[8];
	struct tile *rt;
	struct tile *tr;
	struct tile *bl;
	struct tile *lb;
	bool solid;
	int id;
}Tile;

Tile *layout; // original tile
Tile *search_start;

void TileKill(Tile *tile){
	/*tile->rt = NULL;
	tile->lb = NULL;
	tile->bl = NULL;
	tile->tr = NULL;*/
	tile = NULL;
	delete tile;
}



class TileMerge{
	public:
		Tile *VerticalMerge(Tile *up, Tile *down) {
			/*
			|*********************|
			|                     |
			|-------merge---------|
			|                     |
			|*********************|
			*/
			bool triger = false;
			if (search_start == down){
				triger = true;
			}

			if (up == NULL)
				return down;
			if (down == NULL)
				return up;

			up->point[0] = down->point[0]; up->point[1] = down->point[1];
			up->point[6] = down->point[6]; up->point[7] = down->point[7];

			up->bl = down->bl;
			up->lb = down->lb;

			Tile *temp;
			temp = down->bl;
			if (temp != NULL){
				while (temp != NULL && temp->tr == down) { //down左方的Tile改指向up
					temp->tr = up;
					temp = temp->rt;
				}
			}

			temp = down->tr;
			if (temp != NULL){
				while (temp != NULL && temp->bl == down) { //down右方的Tile改指向up
					temp->bl = up;
					temp = temp->lb;
				}
			}

			temp = down->lb;
			if (temp != NULL){
				while (temp != NULL && temp->rt == down) { //down下方的Tile改指向up
					temp->rt = up;
					temp = temp->tr;
				}
			}

			TileKill(down);

			if (triger == true){
				search_start = up;
			}
			
			return up;
		}

		Tile *HorizontalMerge(Tile *left, Tile *right) {
			/*
			|**********|**********|
			|          |          |
			|        merge        |
			|          |          |
			|**********|**********|
			*/
			bool triger = false;
			if (search_start == right){
				triger = true;
			}

			if (left == NULL)
				return right;
			if (right == NULL)
				return left;

			left->point[4] = right->point[4]; left->point[5] = right->point[5];
			left->point[6] = right->point[6]; left->point[7] = right->point[7];

			left->rt = right->rt;
			left->tr = right->tr;

			Tile *temp;
			temp = right->rt;
			while (temp != NULL && temp->lb == right) { //right上方的Tile改指向left
				temp->lb = left;
				temp = temp->bl;
			}

			temp = right->tr;
			if (temp != NULL){
				while (temp != NULL && temp->bl == right) { //right右方的Tile改指向left
					temp->bl = left;
					temp = temp->lb;
				}
			}

			temp = right->lb;
			if (temp != NULL){
				while (temp != NULL && temp->rt == right) { //right下方的Tile改指向left
					temp->rt = left;
					temp = temp->tr;
				}
			}

			TileKill(right);

			if (triger == true){
				search_start = left;
			}

			return left;
		}

};

class TileCreation{
	TileMerge tm;
	public : 
		void TilePrint(char str[], int i, Tile *til){
			const char s[2][6] = { "space" ,"solid" };
			if (til == NULL) {
				cout << str << " : NULL" << endl;
				return;
			}
			printf("%s %d : ", str, i);
			for (int i = 0; i < 8; i++){
				printf("%d,", til->point[i]);
			}
			cout << s[til->solid];
			printf("\n");
		}

		Tile* SetTile(int a[]) {
			Tile *t;
			t = (struct tile *)malloc(sizeof(struct tile));
			for (int i = 0; i < 8; i++)
				t->point[i] = a[i];
			t->solid = false;
			t->rt = NULL;
			t->tr = NULL;
			t->lb = NULL;
			t->bl = NULL;
			return t;
		}

		Tile* HorizontalSpiltTop(Tile *tile, int y) {
			/*
			|*********************|
			|      OLD->NEW       |
			|---------------------|
			|        OLD          |
			|*********************|
			*/

			if (y - tile->point[3] == 0){ // no space tile
				return tile;
			}

			Tile *New;
			New = (struct tile *)malloc(sizeof(struct tile));

			for (int i = 0; i < 8; i++)              // Tile 座
				New->point[i] = tile->point[i];
			New->point[1] = y; New->point[7] = y;   // 座校正

			tile->point[3] = y; tile->point[5] = y;

			New->solid = tile->solid;



			New->rt = tile->rt;  // New 的 rt  原Tile 指的一
			New->tr = tile->tr;  // New 的 tr  原Tile 指的一
			New->lb = tile;       // New 的 lb 指向 原Tile 

			Tile *temp;
			temp = tile->bl;

			if (temp != NULL) {
				while (temp != NULL && temp->point[3] <= y)  //  找到第一位置高於y的Tile
					temp = temp->rt;  // temp 向上移
			}
			New->bl = temp; //  New 的 bl 指向左方第一位置高於 y 的 Tile
			if (temp != NULL) {
				while (temp != NULL && temp->tr == tile) {  // 所有左方且在y之上的 Tile 的 tr 都改指向 New
					temp->tr = New;
					temp = temp->rt;  // temp 向上移
				}
			}

			temp = tile->rt;
			if (temp != NULL) {
				while (temp != NULL && temp->lb == tile) {  // 所有上方的 Tile 的 lb 都改指向 New
					temp->lb = New;
					temp = temp->bl;  // temp 向左移
				}
			}

			temp = tile->tr;
			if (temp != NULL) {
				while (temp != NULL && temp->point[1] >= y) { //  所有右方且在y之上的 Tile 的 bl 都改指向 New
					temp->bl = New;
					temp = temp->lb;  // temp 向下移
				}
			}
			tile->tr = temp;// 原Tile的CS校正
			tile->rt = New;// 原Tile的CS校正


			return New;
		}

		Tile* HorizontalSpiltBottom(Tile *tile, int y) {
			/*
			|*********************|
			|         OLD         |
			|---------------------|
			|       OLD->NEW      |
			|*********************|
			*/
			Tile *New;
			New = (struct tile *)malloc(sizeof(struct tile));

			if (y - tile->point[1] == 0){ // no space tile
				return tile;
			}

			for (int i = 0; i < 8; i++)              // Tile 座
				New->point[i] = tile->point[i];
			New->point[3] = y; New->point[5] = y;   // 座校正

			tile->point[1] = y; tile->point[7] = y;

			New->solid = tile->solid;

			New->rt = tile;      // New 的 rt 指向 原Tile 
			New->bl = tile->bl;  // New 的 bl  原Tile 指的一
			New->lb = tile->lb;        // New 的 lb 指向 原Tile 

			Tile *temp;
			temp = tile->tr;
			if (temp != NULL) {
				while (temp != NULL && temp->point[1] >= y)  //  找到第一BOTTOM位置低於y的Tile
					temp = temp->lb;  // temp 向下移
			}

			New->tr = temp; //  New 的 tr 指向右方第一BOTTOM位置低於y的Tile
			if (temp != NULL) {
				while (temp != NULL && temp->bl == tile) {  // 所有右方且在y之下的 Tile 的 bl 都改指向 New
					temp->bl = New;
					temp = temp->lb;  // temp 向下移
				}
			}

			temp = tile->lb;
			if (temp != NULL) {
				while (temp != NULL && temp->rt == tile) {  // 所有下方的 Tile 的 rt 都改指向 New
					temp->rt = New;
					temp = temp->tr;  // temp 向右移
				}
			}

			temp = tile->bl;
			if (temp != NULL) {
				while (temp != NULL && temp->point[3] <= y) { //  所有左方且TOP位置在y之下的 Tile 的 tr 都改指向 New
					temp->tr = New;
					temp = temp->rt;  // temp 向上移
				}
			}
			tile->bl = temp; // 原Tile的CS校正
			tile->lb = New;  // 原Tile的CS校正

			return New;
		}

		Tile *VerticalSpiltLeft(Tile *tile, int x) {
			/*
			|**********|**********|
			|          |          |
			|   NEW    |   OLD    |
			|          |          |
			|**********|**********|
			*/

			if (x - tile->point[0]==0){
				return tile;
			}


			Tile *New;
			New = (struct tile *)malloc(sizeof(struct tile));
			for (int i = 0; i < 8; i++)              // Tile 座
				New->point[i] = tile->point[i];
			New->point[4] = x; New->point[6] = x;   // 座校正

			tile->point[0] = x; tile->point[2] = x;

			New->solid = tile->solid;

			New->tr = tile; // New 的 tr 指向 原Tile
			New->bl = tile->bl;
			New->lb = tile->lb;

			Tile *temp;

			temp = tile->rt; // 指向原Tile上方
			while (temp != NULL && temp->point[0] >= x) // 找到第一LEFT在x的左的Tile
				temp = temp->bl;
			New->rt = temp;

			while (temp != NULL && temp->lb == tile) { // 在上方且在x的左的Tile 的 lb 設為 NEW
				temp->lb = New;
				temp = temp->bl;
			}

			temp = tile->bl; // 指向原Tile左方
			if (temp != NULL) {
				while (temp != NULL && temp->tr == tile) { // 所有左方 Tile 的 tr 都改指向 New
					temp->tr = New;
					temp = temp->rt;
				}
			}

			temp = tile->lb; // 指向原Tile下方
			if (temp != NULL) {
				while (temp != NULL && temp->point[4] <= x) { // 所有下方且RIGHT<= x 的 Tile 的 rt 都改指向 New
						temp->rt = New;
					temp = temp->tr;
				}
			}
			tile->lb = temp;
			tile->bl = New;

			return New;
		}

		Tile *VerticalSpiltRight(Tile *tile, int x) {
			/*
			|**********|**********|
			|          |          |
			|   OLD    |   NEW    |
			|          |          |
			|**********|**********|
			*/
			if (x - tile->point[4]==0){
				return tile;
			}

			Tile *New;
			New = (struct tile *)malloc(sizeof(struct tile));
			for (int i = 0; i < 8; i++)              // Tile 座
				New->point[i] = tile->point[i];
			New->point[0] = x; New->point[2] = x;   // 座校正

			tile->point[4] = x; tile->point[6] = x;

			New->solid = tile->solid;

			New->bl = tile; // New 的 bl 指向 原Tile
			New->rt = tile->rt;
			New->tr = tile->tr;

			Tile *temp;

			temp = tile->lb; // 指向原Tile下方
			while (temp != NULL && temp->point[4] <= x) // 找到第一RIGHT在x的右的Tile
				temp = temp->tr;
			New->lb = temp;

			while (temp != NULL && temp->rt == tile){ // 在下方且在x的右的Tile 的 rt 設為 NEW
				temp->rt = New;
				temp = temp->tr;
			}

			temp = tile->tr; // 指向原Tile右方
			if (temp != NULL) {
				while (temp != NULL && temp->bl == tile) { // 所有右方 Tile 的 bl 都改指向 New
					temp->bl = New;
					temp = temp->lb;
				}
			}

			temp = tile->rt; // 指向原Tile上方
			while (temp != NULL && temp->point[0] >= x) { // 所有上方且LEFT >= x 的 Tile 的 lb 都改指向 New
				temp->lb = New;
				temp = temp->bl;
			}
			tile->rt = temp;
			tile->tr = New;

			return New;
		}

		Tile *PointFinding(Tile *tile, int x, int y) {
			Tile *start = tile;
			Tile *temp = NULL;
			bool foundTile = false;
			while (foundTile == false) { // trace tile by "y" , avoid misalignment

				// 剛好在邊上-> 算上、右方
				while (start != NULL && start->point[1] > y){       // target is lower than this tile
					start = start->lb;
				}
				while (start != NULL && start->point[3] <= y){       // target is higher than this tile
					if (start->point[3] == y){
						if (start->rt == NULL)break;  // 超出範圍
					}
					start = start->rt;
				}
				while (start != NULL && start->point[0] > x)       // target is lefter than this tile
					start = start->bl;
				while (start != NULL && start->point[4] <= x){       // target is righter than this tile
					if (start->point[4] == x){
						if (start->tr == NULL)break;  // 超出範圍
					}
					start = start->tr;
				}
				
				if (temp == start){
					TilePrint("start",0,start);
					TilePrint("start->lb", 0, start->lb);
					TilePrint("start->lb->rt", 0, start->lb->rt);
					TilePrint("start->rt", 0, start->rt);
					TilePrint("start->tr", 0, start->tr);
					TilePrint("start->bl", 0, start->bl);
					cout << y << endl;
					cout << (start->point[1] <= y) << endl;
					cout << (start->point[3] > y | start->rt == NULL) << endl;
					cout << (start->point[0] <= x) << endl;
					cout << (start->point[4] > x | start->tr == NULL) << endl;
					cout << foundTile << endl;
					system("pause");
				}

				// double check to avoid misalignment
				foundTile = (start->point[1] <= y ) & 
							(start->point[3] > y  | start->rt == NULL) & 
							(start->point[0] <= x ) &
							(start->point[4] > x  | start->tr == NULL);
				temp = start;
			}
			return start;
		}

		bool AreaSearch(int index,int point[]){

			Tile *StartTile = PointFinding(layout, point[0] , point[3]); // find up
			Tile *EndTile = PointFinding(layout, point[0] , point[1]); // find bottom 
			
			if (StartTile->point[1]== point[3]){ // point finding 找到在邊上的 tile
				StartTile = StartTile->lb;
			}

			Tile *temp = StartTile;
			
			while (temp != EndTile->lb){
				Tile *xTile = temp;
				while (xTile->point[4] <= point[0]){
					xTile = xTile->tr;
				}
				if (xTile->solid == true) // left edge
					return true;
				if (xTile->point[4] >= point[4]){ // this tile cotain right edge
					if (xTile->solid == true){
						return true;
					}
				}
				else{ // this tile do not cotain right edge
					return true;
				}
				temp = xTile->lb;
			}
			return false;
		}

		Tile* TileCreat(int index,int point[]) {
			int htop = point[3];
			int hbot = point[1];
			int vleft = point[0];
			int vright = point[4];


			//Spilt Y
			Tile *StartTile = PointFinding(layout, (vleft + vright)/2, htop);
			if (StartTile->point[1] == point[3]){ // point finding 找到在邊上的 tile
				StartTile = StartTile->lb;
			}
			Tile *Start = HorizontalSpiltTop(StartTile, htop); 
			if (StartTile != Start){  // NOT no space spilt
				Start = Start->lb;
			}
			else{
				TileKill(StartTile);
			}

			Tile *EndTile = PointFinding(layout, (vleft + vright) / 2, hbot);
			Tile *End = HorizontalSpiltBottom(EndTile, hbot);  

			Tile *tempy = Start;
			Tile *xTile = NULL;
			Tile *NewTile = NULL;
			do{
				//找到要切x的Tile
				xTile = tempy;
				while (!(xTile->point[4] >= vright && xTile->point[0] <= vleft)){ 
					xTile = xTile->tr;
				}
					
				if (xTile->point[3] <= hbot) {
					break;
				}
				// Spilt x
				Tile *xleft = VerticalSpiltLeft(xTile, vleft);
				if (xleft == xTile){ // no space split
					TileKill(xleft);
				}
				Tile *xright = VerticalSpiltRight(xTile, vright);
				if (xright == xTile){ // no space split
					TileKill(xright);
				}
				NewTile = tm.VerticalMerge(NewTile, xTile);   // Merge xt and NewTile to -> NewTile
				tempy = xTile->lb;

			} while (xTile != NULL && tempy != NULL && tempy->point[3] > hbot);

			// Merge 左邊相鄰且可合併的 Tile
			if (NewTile->bl != NULL && NewTile->bl->rt != NULL){
				Tile *tempDown = NewTile->bl;
				Tile *tempUp = NewTile->bl->rt;
				while (tempDown != NULL && tempUp != NULL && tempUp->tr == NewTile){
					int width1 = tempUp->point[4] - tempUp->point[2];
					int width2 = tempDown->point[4] - tempDown->point[2];
					if (width1 == width2 && width1 != 0 && tempUp->point[0] == tempDown->point[0]){
						if (tempUp->solid == false && tempDown->solid == false)
							tm.VerticalMerge(tempUp, tempDown);
					}
					tempDown = tempUp;
					tempUp = tempUp->rt;
				}
			}



			// Merge 右邊相鄰且可合併的 Tile //problem
			if (NewTile->tr != NULL && NewTile->tr->lb != NULL){
				Tile *tempUp = NewTile->tr;
				Tile *tempDown = NewTile->tr->lb;
				while (tempDown != NULL && tempUp != NULL && tempUp->bl == NewTile){
					int width1 = tempUp->point[4] - tempUp->point[2];
					int width2 = tempDown->point[4] - tempDown->point[2];
					if (width1 == width2 && width1 != 0 && tempUp->point[0] == tempDown->point[0]){
						if (tempUp->solid == false && tempDown->solid == false){
							tm.VerticalMerge(tempUp, tempDown);
							tempDown = tempUp->lb;
							continue;
						}
					}
					tempUp = tempDown;
					tempDown = tempDown->lb;
				}
			}

			NewTile->solid = true;

			return NewTile;
		}
};

class AreaEnumeration{
	private:
		TileCreation tc;
	public:
		int solid_count;
		int space_count;
		AreaEnumeration(){
			solid_count = 0;
			space_count = 0;
		}
		void enumeration(Tile *tile){
			//tc.TilePrint("Tile", space_count, tile);
			if (tile->solid == true)
				solid_count++;
			else space_count++;
		}
		bool cutBottomTile(Tile *tile, int area[]){
			if (tile->point[1] >= area[1] && tile->point[3] <= area[1]){
				return true;
			}
			else{
				return false;
			}
		}
		void areaEnumeration(int area[]){
			// 1) locate the tile at top left corner
			Tile *start = tc.PointFinding(search_start, area[2], area[3]);
			Tile *end = tc.PointFinding(search_start, area[0], area[1]);

			// step down through all the tiles along the edge
			Tile *temp = start;
			while (temp != NULL && temp->point[1] >= area[1]){
				if (temp->point[1] < area[3])
					R_procedure(temp,area);
				temp = temp->lb;
			}
		}
		void R_procedure(Tile *tile, int area[]){

			// R1)
			enumeration(tile);
			// R2)
			Tile *temp = tile;
			bool cut_currentTile = cutBottomTile(tile, area);
			if (temp->point[4] <= area[4]){
				// R3)
				temp = temp->tr; // pointer to right
				while (temp != NULL && temp->point[3] >= tile->point[1]){
					// R4)
					if (temp->bl == tile){
						R_procedure(temp, area);
					}
					// R5)
					else if (cut_currentTile && cutBottomTile(temp, area)){
						R_procedure(temp, area);
					}
					temp = temp->lb;
				}
			}
		}

};

class TileDeletion{
private:
	TileMerge tm;
	TileCreation tc;
public:
	Tile *HorizontalSpan(int i,Tile *left, Tile *right){
		// 先 spilt 在 merge

		// Spilt : both cut 2 horizontal line
		int x_leftUp = left->point[3]; int x_leftDown = left->point[1];
		int x_rightUp = right->point[3]; int x_rightDown = right->point[1];

		Tile *lu = NULL; Tile *ld = NULL; Tile *ru = NULL; Tile *rd = NULL;

		if (left->point[3] > right->point[3]){
			lu = tc.HorizontalSpiltTop(left, x_rightUp);
			if (i == 235505)cout << "1\n";
		}

		if (left->point[1] < right->point[1]){
			ld = tc.HorizontalSpiltTop(left, x_rightDown);
			if (i == 235505)cout << "2\n";
		}

		if (right->point[3] > left->point[3]){
			ru = tc.HorizontalSpiltTop(right, x_leftUp);
			if (i == 235505)cout << "3\n";
		}

		if (right->point[1] < left->point[1]){
			rd = tc.HorizontalSpiltTop(right, x_leftDown);
			if (i == 235505)cout << "4\n";
		}
		
		// merge left and right tile
		if (ld == NULL && rd == NULL){
			ld = tm.HorizontalMerge(left, right);
			if (i == 235505)cout << "1\n";
		}
		else if (ld != NULL && rd == NULL){
			ld = tm.HorizontalMerge(ld, right);
			if (i == 235505)cout << "2\n";
		}
		else if (ld == NULL && rd != NULL){
			ld = tm.HorizontalMerge(left, rd);
			if (i == 235505)cout << "3\n";
		}
		else{ // ld != NULL && rd != NULL
			ld = tm.HorizontalMerge(ld, rd);
			if (i == 235505)cout << "4\n";
		}

		if (i == 235505){
			tc.TilePrint("ld : ", 0, ld);
		}

		// merge up down if possible
		Tile *temp = ld->rt;
		if (temp != NULL){
			int width1 = temp->point[4] - temp->point[0];
			int width2 = ld->point[4] - ld->point[0];
			if (temp != NULL && temp->point[0] == ld->point[0] && width1 == width2){
				if (temp->solid == false && ld->solid == false){
					ld = tm.VerticalMerge(temp, ld);
				}
					
			}
		}

		temp = ld->lb;

		if (i == 235505){
			tc.TilePrint("temp : ", 0, temp);
		}

		if (temp != NULL){
			int width1 = temp->point[4] - temp->point[0];
			int width2 = ld->point[4] - ld->point[0];
			if (temp != NULL && temp->point[0] == ld->point[0] && width1 == width2){
				if (temp->solid == false && ld->solid == false){
					ld = tm.VerticalMerge(ld, temp);
				}
			}
		}

		if (i == 235505){
			tc.TilePrint("ld : ", 1, ld );
			tc.TilePrint("temp  : ", 1, temp);
			tc.TilePrint("search_start  : ", 1, search_start);
			temp = tc.PointFinding(search_start, 1, 1);
			tc.TilePrint("temp : ", 1, temp);
		}

		return ld;
	}
	void TileDelete(int i,Tile *dtile){
		// 1) change dead tile to "space"
		dtile->solid = false;
		int bottom = dtile->point[1];
		int height = dtile->point[3];

		// 2) trace through right edge
		Tile *temp = dtile->tr;
		while (temp != NULL && temp->bl == dtile){ // trace through right edge
			Tile *Next = temp->lb;
			if (temp->solid==false && dtile->solid==false)
				Tile *merge = HorizontalSpan(i,dtile, temp); // 3) spilt and merge
			temp = Next;
		}


		//4) trace through left edge , but dtile's stitching are change
		// 就是這一段讓結構變了!!!
		temp = dtile->bl;
		while (temp != NULL && temp->point[3] <= height){ // trace through left edge without stitching
			Tile *tp = temp->tr;
			Tile *Next = temp->rt;
			int height2 = temp->point[1];
			while (tp != NULL && tp->point[3] > height2){            // 4.2) trace through right edge
				Tile *next = tp->lb;
				if (temp->solid == false && tp->solid == false)
					Tile *merge = HorizontalSpan(i,temp,tp); // 4.3) spilt and merge
				tp = next;
			}
			temp = Next;
		}

	}
};

Tile *PFinding2(Tile *tile, int x, int y) {
	TileCreation tc;
	Tile *start = tile; // tp
	if (y < start->point[1]){
		do{
			start = start->lb;
		} while (y < start->point[1]);
	}
	else{
		while (y >= start->point[3])
		{
			start = start->rt;
		}
	}
	if (x < start->point[0])
		do
		{
			do {
				start = start->bl;
			} while (x < start->point[0]);

			if (y < start->point[3]) break;

			do {
				start = start->rt;
			} while (y >= start->point[3]);

		} while (x < start->point[0]);
	else{
		while (x >= start->point[4])
		{
			do { 
				start = start->tr; 
			} while (x >= start->point[4]);

			if (y >= start->point[1]) break;

			do{ 
				start = start->lb;
			} while (y < start->point[1]);
		}
	}

	return start;
}

Tile *PFinding(Tile *tile, int x, int y) {
	TileCreation tc;
	Tile *start = tile;
	Tile *temp = NULL;
	bool foundTile = false;
	while (foundTile == false) { // trace tile by "y" , avoid misalignment

		// 剛好在邊上-> 算上、右方
		while (start != NULL && start->point[1] > y){       // target is lower than this tile
			start = start->lb;
		}
		//tc.TilePrint("start", 0, start);
		while (start != NULL && start->point[3] <= y){       // target is higher than this tile
			if (start->point[3] == y){
				if (start->rt == NULL)break;  // 超出範圍
			}
			start = start->rt;
		}
		//tc.TilePrint("start", 0, start);
		while (start != NULL && start->point[0] > x){       // target is lefter than this tile

			if (start->point[0] > x)
				start = start->bl;

			if (start->point[0] <= x){
				if(start->point[3] <= y){ 
					start = start->rt;
				}
			}
		}
		//tc.TilePrint("start", 0, start);
		while (start != NULL && start->point[4] <= x){       // target is righter than this tile
			if (start->point[4] == x){
				if (start->tr == NULL)break;  // 超出範圍
			}
			if (start->point[4] <= x)
				start = start->tr;

			if (start->point[4] > x){
				if (start->point[1] > y){
					start = start->lb;
					continue;
				}
			}
		}
		/*tc.TilePrint("start", 0, start);

		system("pause");*/

		// double check to avoid misalignment
		foundTile = (start->point[1] <= y) &
			(start->point[3] > y | start->rt == NULL) &
			(start->point[0] <= x) &
			(start->point[4] > x | start->tr == NULL);
		temp = start;
	}
	return start;
}

class Layout{
	int cellNum;
	int **index;
	bool *index_status; // 紀錄tile是否有成功加入
public:
	Layout(FILE *fp){
		int a[8];
		cout << "Loading...." << endl; 
		while (fscanf(fp, "%d %d %d %d %d %d %d %d", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7]) != EOF){
			cellNum++; // count cell num
		}

		fseek(fp, 0, SEEK_SET); // file pointer back to 0

		// Double loading time to reduce memory waste
		//File index loading
		index = new int *[cellNum];

		for (int i = 0; i<cellNum; i++)
			index[i] = new int[8];

		index_status = new bool[cellNum];

		for (int i = 0; i < cellNum; i++){
			for (int j = 0; j < 8;j++)
				fscanf(fp, "%d", &index[i][j]);
		}
		cout << "Loading Complete" << endl;
		cout << "Solid Tile Num : "<< cellNum << endl;
	}

	void Bound(){
		// According to point C
		layout_width = 0;
		layout_height = 0;

		for (int i = 0; i < cellNum; i++){
			if (index[i][4] > layout_width){
				layout_width = index[i][4];
			}
			if (index[i][5] > layout_height){
				layout_height = index[i][5];
			}
		}
		lay[3] = layout_height;
		lay[4] = layout_width;
		lay[5] = layout_height;
		lay[6] = layout_width;
		cout << "Bound : (" << layout_width << ","<< layout_height<< ")\n" ;

	}
	void OutofRange_check(Tile *add){
		if (add->rt != NULL){
			if (add->rt->point[3] - add->rt->point[1] == 0 && add->rt->point[1] == layout_height){ // Top out of range
				add->rt = NULL;
			}
		}
		if (add->lb != NULL){
			if (add->lb->point[3] - add->lb->point[1] == 0 && add->lb->point[1] == layout_height){ // Bottom out of range
				add->lb = NULL;
			}
		}
		if (add->tr != NULL){
			if (add->tr->point[4] - add->tr->point[0] == 0 && add->tr->point[0] == layout_width){ // Right out of range
				add->tr = NULL;
			}
		}
		if (add->bl != NULL){
			if (add->bl->point[4] - add->bl->point[0] == 0 && add->bl->point[0] == layout_width){ // Left out of range
				add->bl = NULL;
			}
		}
	}
	void OP1(){
		/*OP1 : Read all rectangles and save them in a list.
		Determine the boundaries of your corner stitching plane based on all input rectangles*/
		Bound(); // Find and Set layout bound
	}
	void OP2(TileCreation tc){
		int count = 0;
		cout << "Tile Creation Start...." << endl;
		for (int i = 0; i < cellNum; i++){
			if (tc.AreaSearch(i,index[i]) == true){
				//cout << "Tile "<< i << " occupy reject" << endl;
				index_status[i] == false;
				continue;
			}
			
			Tile* add;
			add = tc.TileCreat(i,index[i]);
			//tc.TilePrint("Tile",i,add);
			OutofRange_check(add);

			/*tc.TilePrint("Tile->rt", i, add->rt);
			tc.TilePrint("Tile->tr", i, add->tr);
			tc.TilePrint("Tile->lb", i, add->lb);
			tc.TilePrint("Tile->bl", i, add->bl);*/

			add->id = i;
			index_status[i] = true;
			search_start = add;
			count++;
		}
		cout << "Tile Creation Complete" << endl;
		cout << "\nSoild Tile count = " << count << endl;
	}
	void OP3(){
		int area[8] = {0,0,0,10,80,10,80,0};
		AreaEnumeration AE;
		cout << "Enumeration Start...." << endl;
		AE.areaEnumeration(lay);
		cout << "Enumeration Complete" << endl;
		cout << "\nEnumeration Solid count : " << AE.solid_count << endl;
		cout << "\nEnumeration Space count : " << AE.space_count << endl;
	}

	void OP4(TileCreation tc){
		TileDeletion td;
		Tile *dtile = NULL;
		// 已確認刪除前，所有tile皆可被PointFinding找到。
		// Tile Delete 錯誤導致搜尋不到
		/*cout << "checking Start...." << endl;
		for (int i = 0; i < cellNum; i++){
			if (index_status[i] == true){
				tc.PointFinding(search_start, (index[i][0] + index[i][4]) / 2, (index[i][1] + index[i][3]) / 2);
			}
		}
		cout << "checking Complete" << endl;*/

		cout << "Tile deletion Start...." << endl;
		for (int i = 0; i < cellNum; i++){
			if (index_status[i] == true){

				if (i == 50493){
					cout << "index-50493 : ";
					for (int j = 0; j < 8; j++){
						printf("%d,", index[i][j]);
					}
					cout << "\n";
					tc.TilePrint("search_start  : ", 0, search_start);
					dtile = PFinding(search_start, index[i][0], index[i][1]);
					system("pause");
				}

				dtile = PFinding(search_start, (index[i][0] + index[i][4]) / 2, (index[i][1] + index[i][3]) / 2);

				while (dtile == search_start){
					search_start = tc.PointFinding(search_start, 0, 0);
					/*tc.TilePrint("dtile  : ", 0, dtile);
					tc.TilePrint("search_start  : ", 0, search_start);
					system("pause");*/
				}
				
				td.TileDelete(i,dtile);

				
				if (i % 50000==0)printf("%d\n", i);
			}
		}
		cout << "Tile deletion Complete" << endl;

	}
};

int main(){

	clock_t start, stop;

	start = clock();

	FILE *fp = fopen("RSA/MT1org.trp", "r"); //input file
	TileCreation tc;
	Layout lyt(fp);
	
	/*Operation 1~4*/
	lyt.OP1();
	layout = tc.SetTile(lay);
	lyt.OP2(tc);
	lyt.OP3();
	lyt.OP4(tc);
	printf("After All tile delete : \n");
	lyt.OP3();
	Tile *l = tc.PointFinding(search_start, 1, 1);
	tc.TilePrint("l  : ", 0, l);
	stop = clock();


	printf("\nElapsed %.2lf seconds\n", (double)(stop - start) / CLOCKS_PER_SEC);
	system("pause");

	return 0;
}

