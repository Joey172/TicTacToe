#pragma once
enum Tile {
  NONE = 0,
  CROSS,
  CIRCLE,
  TRIANGLE,
};

//typedef unsigned Tile;

struct Point {
  unsigned x;
  unsigned y;
};

struct play_t {
  Point majorPos;
  Point minorPos;
  Tile tile;
  bool wasFreePlay;
};