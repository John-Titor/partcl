#ifndef _TCL_H
#define _TCL_H

#include <stdbool.h>

struct tcl_env;
struct tcl_cmd;
typedef char tcl_value_t;
struct tcl {
  struct tcl_env *env;
  struct tcl_cmd *cmds;
  tcl_value_t *result;
};


/* =========================================================================
    High level interface
   ========================================================================= */

/** tcl_init() initializes the interpreter context.
 *  \param tcl      The interpreter context.
 */
void tcl_init(struct tcl *tcl);

/** tcl_destroy() cleans up the interpreter context, frees all memory.
 *  \param tcl      The interpreter context.
 */
void tcl_destroy(struct tcl *tcl);

/** tcl_eval() runs a script stored in a memory buffer.
 *  \param tcl      The interpreter context.
 *  \param script   The buffer with the script.
 *  \param length   The length of the buffer.
 *
 *  \return 0 on error, 1 on success; other non-zero codes are used internally
 *          (and may be assumed "success").
 *
 *  \note On completion (of a successful run), the output of the script is
 *        stored in the "result" field of the "tcl" context.
 */
int tcl_eval(struct tcl *tcl, const char *script, size_t length);


/* =========================================================================
    Values
   ========================================================================= */

/** tcl_string() returns a pointer to the start of the contents of a value
 *  (where the value is assumed to be a string).
 *  \param v        The value.
 *
 *  \return A pointer to the buffer.
 */
const char *tcl_string(tcl_value_t *v);

/** tcl_int() returns the value of a variable after parsing it as an integer
 *  value. The function supports decimal, octal and dexadecimal notation.
 *  \param v        The value.
 *
 *  \return The numeric value of the parameter, or 0 on error.
 */
long tcl_int(tcl_value_t *v);

/** tcl_length() returns the length of the contents of the value in bytes.
 *  \param v        The value.
 *
 *  \return The number of bytes in the buffer of the value.
 */
int tcl_length(tcl_value_t *v);


/* =========================================================================
    Low level interface
   ========================================================================= */

/** tcl_next() gets the next token from the stream (lexical analysis).
 *  \param script   The buffer with the script.
 *  \param length   The size of the "script" buffer.
 *  \param from     [out] Is set to the start of the token, on return.
 *  \param to       [out] Is set to just behind the end of the token, on return.
 *  \param quote    [out] Is set to true when inside a quoted string, or false if
 *                  otherwise.
 *
 *  \return The token type (0 = parsing error).
 */
int tcl_next(const char *script, size_t length, const char **from, const char **to, bool *quote);


/* =========================================================================
    User commands
   ========================================================================= */

typedef int (*tcl_cmd_fn_t)(struct tcl *tcl, tcl_value_t *args, void *user);

/** tcl_register() registers a C function to the ParTcl command set.
 *  \param tcl      The interpreter context.
 *  \param name     The name of the command.
 *  \param fn       The function pointer.
 *  \param arity    The number of parameters of the command, which includes the
 *                  command name itself. Set this to zero for a variable
 *                  argument list.
 *  \param user     A user value (which is passed to the C function).
 */
void tcl_register(struct tcl *tcl, const char *name, tcl_cmd_fn_t fn, int arity, void *user);


/* =========================================================================
    COBS encoding
   ========================================================================= */

/** tcl_cobs_encode() encodes a binary data block such that no embedded zero
 *  bytes occur in the middle; a zero-terminator is appended to the end, though
 *  (COBS encoding).
 *  \param bindata  The block with binary data.
 *  \param length   [in/out] On input, the length of the bindata buffer; on
 *                  output, the size of the output buffer.
 *
 *  \return A pointer to a buffer with the encoded data (or NULL on failure).
 *
 *  \note The returned memory block must be deallocated with free().
 *
 *  \note The returned length is the same as strlen() of the returned buffer,
 *        plus 1 for the zero-terminator.
 */
const char *tcl_cobs_encode(const char *bindata, size_t *length);

/** tcl_cobs_decode() decodes an COBS-encoded block, and returns the original
 *  binary encoded block.
 *  \param asciiz   The block with encoded data.
 *  \param length   [in/out] On input, the length of the asciiz buffer; on
 *                  output, the size of the output buffer.
 *
 *  \return A pointer to a buffer with the decoded data (or NULL on failure).
 *
 *  \note The returned memory block must be deallocated with free().
 */
const char *tcl_cobs_decode(const char *asciiz, size_t *length);

#endif /* _TCL_H */
