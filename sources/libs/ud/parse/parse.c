#include <brutal/debug.h>
#include <brutal/ds.h>
#include <brutal/io.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>
#include "brutal/parse/lex.h"
#include "ud/ast/expr.h"

UdVal get_val_from_lexeme(Lexeme lexeme)
{
    UdVal ret = {};

    switch (lexeme.type)
    {
    case UDLEX_INTEGER:
    {
        ret.type = UD_VAL_INT;
        ret.int_ = str_to_number(lexeme.str);
        break;
    }
    case UDLEX_STRING:
    {
        ret.type = UD_VAL_STR;
        ret.string = lexeme.str;
        break;
    }

    default:
        log$("Unknown lexeme '{}'", lexeme.str);
        break;
    }

    return ret;
}

// Pure crap
// DON'T USE
UdBinOp ud_parse_bin_op(Lex *lex, Alloc *alloc)
{
    UdBinOp ret = {};

    lex->head -= 1;

    while (lex_curr(lex).type == UDLEX_WHITESPACE)
    {
        lex->head -= 1;
    }

    UdExpr left = ud_parse_expr(lex, alloc).expr;

    log$("{}", left.const_.int_);

    lex_next(lex);
    lex_next(lex);

    ud_parse_whitespace(lex);

    UdExpr right = ud_parse_expr(lex, alloc).expr;

    log$("{}", right.const_.int_);

    ret.left = alloc_malloc(alloc, sizeof(UdExpr));
    ret.right = alloc_malloc(alloc, sizeof(UdExpr));

    mem_cpy(ret.left, &left, sizeof(UdExpr));
    mem_cpy(ret.right, &right, sizeof(UdExpr));

    return ret;
}

UdAstNode ud_parse_expr(Lex *lex, Alloc *alloc)
{
    UdAstNode ret = {};

    if (lex_curr(lex).type == UDLEX_INTEGER || lex_curr(lex).type == UDLEX_STRING)
    {
        ret.expr.type = UD_EXPR_CONSTANT;

        ret.expr.const_ = get_val_from_lexeme(lex_curr(lex));
    }

    else if (lex_curr(lex).type == UDLEX_IDENT && lex_peek(lex, 1).type != UDLEX_LPAREN)
    {
        ret.expr.type = UD_EXPR_REFERENCE;
        ret.expr.reference = lex_curr(lex).str;
    }

    else if (lex_curr(lex).type == UDLEX_IDENT && lex_peek(lex, 1).type == UDLEX_LPAREN)
    {
        ret.expr.type = UD_EXPR_FUNC_CALL;
        ret.expr.func_call = ud_parse_func_call(lex, alloc);
    }

    else
    {
        //    lex_next(lex);
    }

    return ret;
}

UdAstNode ud_parse(Lex *lex, Alloc *alloc)
{

    UdAstNode ret = {};

    if (lex_curr_type(lex) == LEXEME_EOF)
    {
        return ret;
    }

    if (lex_curr(lex).type != UDLEX_WHITESPACE && lex_curr(lex).type != UDLEX_COMMENT && lex_curr(lex).type != LEXEME_EOF)
    {
        UdAstNode out = ud_parse_decl(lex, alloc);

        if (out.expr.decl.type == UD_DECL_NONE)
        {
            out = ud_parse_expr(lex, alloc);

            lex_next(lex);

            ud_parse_whitespace(lex);

            ud_expect(lex, UDLEX_SEMICOLON);

            ud_parse_whitespace(lex);
        }

        ret = out;
    }

    else if (lex_curr(lex).type != LEXEME_EOF)
    {
        lex_next(lex);

        ret = ud_parse(lex, alloc);
    }

    return ret;
}

UdAst ud_parse_file(Lex *lex, Alloc *alloc)
{
    UdAst ret;

    vec_init(&ret, alloc);

    while (lex_curr(lex).type != LEXEME_EOF)
    {
        UdAstNode node = ud_parse(lex, alloc);

        if (node.expr.type != UD_EXPR_NIL)
        {
            vec_push(&ret, node);
        }
    }

    return ret;
}