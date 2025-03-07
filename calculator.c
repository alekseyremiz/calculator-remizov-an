#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#define MAX_BUFFER 1024

static const double VALUE_LIMIT = 2e9;
static const char *cursor = NULL;
static bool use_floats = false;

static void throw_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

static void trim_spaces(void) {
    while (*cursor && isspace((unsigned char)*cursor)) {
        cursor++;
    }
}

static bool is_valid_character(char ch) {
    return (isdigit((unsigned char)ch) || ch == '(' || ch == ')' || 
            ch == '*' || ch == '+' || ch == '-' || ch == '/' || isspace((unsigned char)ch));
}

static double extract_number(void) {
    trim_spaces();
    if (!isdigit((unsigned char)*cursor)) {
        throw_error("Expected a number");
    }
    double num = 0.0;
    while (isdigit((unsigned char)*cursor)) {
        num = num * 10.0 + (*cursor - '0');
        cursor++;
        if (num > VALUE_LIMIT) {
            throw_error("Number exceeds allowed range");
        }
    }
    return num;
}

static double evaluate_expression(void);
static double evaluate_term(void);
static double evaluate_factor(void);

static void validate_range(double x) {
    if (x < -VALUE_LIMIT || x > VALUE_LIMIT) {
        throw_error("Value out of range");
    }
}

static bool is_whole_number(double x) {
    return fabs(x - round(x)) < 1e-9;
}

static double integer_divide(double lhs, double rhs) {
    if (fabs(rhs) < 1e-4) {
        throw_error("Division by zero or near-zero");
    }
    double result = floor(lhs / rhs);
    validate_range(result);
    return result;
}

static double floating_divide(double lhs, double rhs) {
    if (fabs(rhs) < 1e-4) {
        throw_error("Division by zero or near-zero");
    }
    double result = lhs / rhs;
    validate_range(result);
    return result;
}

static double evaluate_factor(void) {
    trim_spaces();
    double val = 0.0;
    bool negative = false;
    while (*cursor == '+' || *cursor == '-') {
        if (*cursor == '-') {
            negative = !negative;
        }
        cursor++;
        trim_spaces();
    }
    if (*cursor == '(') {
        cursor++;
        val = evaluate_expression();
        trim_spaces();
        if (*cursor == ')') {
            cursor++;
        } else {
            throw_error("Missing closing parenthesis");
        }
    } else {
        val = extract_number();
    }
    if (negative) {
        val = -val;
    }
    validate_range(val);
    return val;
}

static double evaluate_term(void) {
    double val = evaluate_factor();
    trim_spaces();
    while (*cursor == '*' || *cursor == '/') {
        char op = *cursor;
        cursor++;
        trim_spaces();
        double right = evaluate_factor();
        trim_spaces();
        if (op == '*') {
            val *= right;
            validate_range(val);
            if (!use_floats && !is_whole_number(val)) {
                throw_error("Non-integer result in integer mode");
            }
        } else {
            val = use_floats ? floating_divide(val, right) : integer_divide(val, right);
        }
    }
    return val;
}

static double evaluate_expression(void) {
    double val = evaluate_term();
    trim_spaces();
    while (*cursor == '+' || *cursor == '-') {
        char op = *cursor;
        cursor++;
        trim_spaces();
        double right = evaluate_term();
        trim_spaces();
        val = (op == '+') ? val + right : val - right;
        validate_range(val);
        if (!use_floats && !is_whole_number(val)) {
            throw_error("Non-integer result in integer mode");
        }
    }
    return val;
}

static void sanitize_input(char *str) {
    char *dest = str;
    for (char *src = str; *src; src++) {
        if (!isspace((unsigned char)*src)) {
            *dest++ = *src;
        }
    }
    *dest = '\0';
}

static void check_valid_chars(const char *str) {
    while (*str) {
        if (!is_valid_character(*str)) {
            throw_error("Invalid character in input");
        }
        str++;
    }
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--float") == 0) {
            use_floats = true;
        } else {
            throw_error("Unknown argument");
        }
    }
    char buffer[MAX_BUFFER];
    size_t len = 0;
    int c;
    while ((c = getchar()) != EOF && len < MAX_BUFFER - 1) {
        buffer[len++] = (char)c;
    }
    if (c != EOF && len >= MAX_BUFFER - 1) {
        throw_error("Input exceeds allowed size");
    }
    buffer[len] = '\0';
    check_valid_chars(buffer);
    sanitize_input(buffer);
    if (buffer[0] == '\0') {
        throw_error("Empty input");
    }
    cursor = buffer;
    double result = evaluate_expression();
    trim_spaces();
    if (*cursor != '\0') {
        throw_error("Unexpected characters after expression");
    }
    if (use_floats) {
        printf("%.4f\n", result);
    } else {
        if (!is_whole_number(result)) {
            throw_error("Final result not an integer");
        }
        printf("%lld\n", (long long)round(result));
    }
    return 0;
}
