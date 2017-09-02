# Scribble API

---

### Basic argument types

**Points** are two numbers, named x and y
* Scheme example: `'(100 50)`
* Lua example: `{100, 50}`

**Rectangles** are four numbers, x/y/w/h, for a top-left coordinate and width/height.

* Scheme example: `'(0 0 32 32)`
* Lua example: `{0, 0, 32, 32}`

**Colors** are four numbers, r/g/b/a, for red/green/blue/alpha. All range from 0-255, alpha is optional and defaults to 255 (fully opaque).

* Scheme example: `'(0 0 255)`
* Lua example: `{0, 0, 255}`

### Entities

* Calling a function to add a entity will be responded to by an index number
* Indexes may be repeated after a while if things are deleted
* Indexes can be used to alter properties of entities, or delete them

### Drawing lines

* p1, p2 are points, color is a color
* color is optional, defaults to `'(255 255 255 255)`
* Scheme example: `(line #:p1 '(0 0) #:p2 '(100 100) #:color '(255 0 0))`
* Lua: `line{p1={0, 0}, p2={100, 100}, color={255, 0, 0}}`

### Drawing rectangles

* First argument is a rectangle, color is a color, fill is either 0 or 1
* x/y/w/h for the first argument can be edited / tweened directly
* color is optional, defaults to `'(255 255 255 255)`
* Scheme example: `(rect '(10 10 32 32) #:fill 1 #:color '(120 120 120 64))`
* Lua example: `rect{{10, 10, 32, 32}, fill=1, color={120, 120, 120, 64}}`

### Editing properties

* Entity properties can be changed with the edit function
* Arguments are the index of the entity, the property name to change (as a string), and the new value
* Where the property is a structure like a point or color, use dots to refer to one member
* The entire structure can't be changed at once, only one property at a time
* Scheme example: `(edit 0 "p2.y" 200)`
* Lua example: `edit(0, 'p2.y', 200)`

### Animation (Tweening)

* Animation of property changes can be done on simple properties

### Deleting entities

### Loading textures

### Drawing sprites