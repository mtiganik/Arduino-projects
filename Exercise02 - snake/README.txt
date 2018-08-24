I like this snake game because of the nice logic that is behind it

Logic is really simple. I'm using 2D int array. Head position value is always the length of the snake. If snake moves, then every tail element is value is decreased by one. If array value reaches to 0, then there is no more tail anymore. Food value is -2.

For example when food is up and snake is down, then array looks like this:

MAP[][] =
{
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, -2, 0, 0, 0, 0, 0 };   // Food is here
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 1, 2, 3, 4, 5, 0 }     // Snake is here
};

And when user now presses counterClockwise button, then 2D array will look:

MAP[][] =
{
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, -2, 0, 0, 0, 0, 0 };   // Food is here
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 0, 0 };
  { 0, 0, 0, 0, 0, 5, 0 };    // Snake head is here
  { 0, 0, 1, 2, 3, 4, 0 }     
};
