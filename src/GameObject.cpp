
#include "Classes.h"

Vector2 GameObject::GetPosition() {
    return Position;
}

Vector2 GameObject::GetSize() {
    return Size;
}

void GameObject::Translate(Vector2 Direction) {
    Position.x += Direction.x;
    Position.y += Direction.y;
}

void GameObject::Rotate(float angle) {
    Rotation += angle;
}

void GameObject::AddSize(Vector2 addSize) {
    Size.x += addSize.x;
    Size.y += addSize.y;
}

void GameObject::SetPosition(Vector2 newPosition) {
    Position = newPosition;
}

void GameObject::SetRotation(float newRotation) {
    Rotation = newRotation;
}

void GameObject::SetSize(Vector2 newSize) {
    Size = newSize;
}