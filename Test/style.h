#ifndef STYLE_H
#define STYLE_H

// Intensities
#define BRIGHT  1
#define DIM     2
#define NORMAL  22

// Colours
#define BLACK   30
#define RED     31
#define GREEN   32
#define YELLOW  33
#define BLUE    34
#define MAGENTA 35
#define CYAN    36
#define WHITE   37
#define DEFAULT 39

#define stringize(expr) #expr
#define escape(code) "\x1b[" stringize(code) "m"

// Intensity Setters
#define bright(string) escape(BRIGHT) string escape(NORMAL)
#define dim(string) escape(DIM) string escape(NORMAL)
#define colour(code, string) escape(code) string escape(DEFAULT)

// Colour Setters
#define black(string) colour(BLACK, string)
#define red(string) colour(RED, string)
#define green(string) colour(GREEN, string)
#define yellow(string) colour(YELLOW, string)
#define blue(string) colour(BLUE, string)
#define magenta(string) colour(MAGENTA, string)
#define cyan(string) colour(CYAN, string)
#define white(string) colour(WHITE, string)

#endif // !STYLE_H
