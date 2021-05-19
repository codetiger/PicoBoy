enum Keys {
    None,
    Right,
    Left,
    Up,
    Down,
    A,
    B,
    Select,
    Start
};

enum Type {
    Button,
    Direction
};

class Input {
public:
    void ButtonPressed(Keys button);
    void ButtonReleased(Keys button);
private:
};