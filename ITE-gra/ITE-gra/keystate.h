#ifndef KEYSTATE_H
#define KEYSTATE_H

struct KeyState {
    int key;
    bool active;

    KeyState(int k = 0) : key(k), active(false) {}
};

#endif