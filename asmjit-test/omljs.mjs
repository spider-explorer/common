import { oml2ast } from "./oml2ast.mjs";

function is_array(x) {
    return (x instanceof Array);
}

function is_quoted(x) {
    if (!is_array(x)) return false;
    if (x.length === 0) return false;
    return x[0] === "`";
}

function to_def(ast) {
    if (!is_array(ast)) return null;
    if (ast.length === 0) return null;
    switch (ast[0]) {
        case "def": {
            if (ast.length < 2) throw new Error("sysntax error");
            let ast1 = ast[1];
            let ast2 = ast.length === 2 ? null : ast[2];
            return ["def", ast1, ast2];
        }
        case "defvar": {
            if (ast.length < 2) throw new Error("sysntax error");
            let ast1 = ast[1];
            let ast2 = ast.length === 2 ? null : ast[2];
            return ["def", ast1, ast2];
        }
        case "defun": {
            let new_ast = ast.slice(3);
            new_ast.unshift(ast[2]);
            new_ast.unshift("fn");
            return ["def", ast[1], new_ast];
        }
        case "define": {
            if (ast[1] instanceof Array) {
                if (ast.length < 2) throw new Error("sysntax error");
                let new_ast = ast.slice(2);
                new_ast.unshift(ast[1].slice(1));
                new_ast.unshift("fn");
                return ["def", ast[1][0], new_ast];
            }
            else {
                if (ast.length < 2) throw new Error("sysntax error");
                let ast1 = ast[1];
                let ast2 = ast.length === 2 ? null : ast[2];
                return ["def", ast1, ast2];
            }
        }
        default:
            return null;
    }
}

function compile_body_helper(body) {
    if (body.length === 0) return null;
    let result = "(";
    for (let i = 0; i < body.length; i++) {
        if (i > 0)
            result += ",";
        let def = to_def(body[i]);
        if (def !== null) {
            let let_ast = ["let", [[def[1], def[2]]], ...body.slice(i + 1)];
            return result + compile_ast(let_ast) + ")";
        }
        result += compile_ast(body[i]);
    }
    return result + ")";
}

function compile_body(ast, start) {
    let body = [];
    for (let i = start; i < ast.length; i++) {
        body.push(ast[i]);
    }
    return compile_body_helper(body);
}

function compile_body1(ast) {
    if (to_def(ast) !== null) throw Error("def is not allowed here");
    return compile_ast(ast);
}

function compile_ast(ast) {
    if (ast === undefined)
        return "undefined";
    if (!ast) {
        return JSON.stringify(ast);
    }
    if (typeof ast === "string") {
        if (ast.match(/^:.+$/) || ast.match(/^#.+$/))
            return JSON.stringify(ast);
        return ast;
    }
    if (!(ast instanceof Array)) {
        return ast.toString();
    }
    if (ast.length === 0)
        return "[]";
    switch (ast[0]) {
        case "`":
            return JSON.stringify(ast[1]);
        case "@":
            return ast[1];
        case "begin":
            return compile_body(ast, 1);
        case "case": {
            let result = "(function(){switch(" + compile_body1(ast[1]) + "){";
            let rest = ast.slice(2);
            rest.forEach((x) => {
                let val = x[0];
                switch (val) {
                    case ":else":
                    case "else":
                    case "otherwise":
                    case ":otherwise":
                        result += "default:";
                        break;
                    default:
                        if (val instanceof Array && val[0] === "`")
                            val = val[1];
                        result += "case " + JSON.stringify(val) + ":";
                }
                let body = compile_body(x, 1);
                result += "return " + body + ";";
            });
            result += "}return null})()";
            return result;
        }
        case "_cond": {
            function _cond_builder(rest) {
                if (rest.length === 0)
                    return null;
                let condition = rest.shift();
                let action = rest.shift();
                switch (condition) {
                    case true:
                    case ":else":
                    case "else":
                    case "otherwise":
                    case ":otherwise":
                        return action;
                }
                return ["if", condition, action, _cond_builder(rest)];
            }
            ast = _cond_builder(ast.slice(1));
            return compile_ast(ast);
        }
        case "cond": {
            let new_ast = [];
            ast.slice(1).forEach((x) => {
                new_ast.push(x[0]);
                new_ast.push(["begin"].concat(x.slice(1)));
            });
            new_ast.unshift("_cond");
            return compile_ast(new_ast);
        }
        case "dec!":
        case "inc!":
            let sign = ast[0] === "dec!" || ast[0] === "dec" ? "-" : "+";
            let val = ast.length < 3 ? 1 : compile_ast(ast[2]);
            return compile_body1(ast[1]) + sign + "=" + val;
        case "def": {
            ast = to_def(ast);
            //return "let " + ast[1] + "=" + compile_body1(ast[2]);
            return "globalThis." + ast[1] + "=" + compile_body1(ast[2]);
        }
        case "define": case "defun": case "defvar": {
            ast = to_def(ast);
            return compile_ast(ast);
        }
        case "do":
        case "do*":
            return compile_do(ast);
        case "fn":
        case "lambda": {
            let args = "(";
            for (let i = 0; i < ast[1].length; i++) {
                if (i > 0)
                    args += ",";
                args += ast[1][i];
            }
            args += ")";
            if (ast.length < 3)
                return "function" + args + "{}";
            return "function" + args + "{return " + compile_body(ast, 2) + "}";
        }
        case "dotimes": {
            let ast1 = ast[1];
            if (!is_array(ast1) || is_quoted(ast1))
                ast1 = ["$index", ast1];
            else if (ast1.length < 2)
                throw new Error("syntax error");
            let result_exp = ast1.length < 3 ? "null" : ast1[2];
            let bind = [
                ["__dotimes_cnt__", ast1[1]],
                ["__dotimes_idx__", 0, ["+", "__dotimes_idx__", 1]],
                [ast1[0], "__dotimes_idx__", "__dotimes_idx__"],
            ];
            let exit = [[">=", "__dotimes_idx__", "__dotimes_cnt__"], result_exp];
            ast = ["do*", bind, exit].concat(ast.slice(2));
            return compile_ast(ast);
        }
        case "length": {
            if (ast.length != 2) return new Error("syntax error");
            return "(" + compile_body1(ast[1]) + ").length";
        }
        case "prop-get": {
            if (ast.length != 3) return new Error("syntax error");
            return compile_body1(ast[1]) + "[" + compile_body1(ast[2]) + "]";
        }
        case "prop-set!": {
            if (ast.length != 4) return new Error("syntax error");
            return compile_body1(ast[1]) + "[" + compile_body1(ast[2]) + "]=" + compile_body1(ast[3]);
        }
        case "dolist": {
            let ast1 = ast[1];
            if (!is_array(ast1) || is_quoted(ast1))
                ast1 = ["$item", ast1];
            else if (ast1.length < 2)
                throw new Error("syntax error");
            let result_exp = ast1.length < 3 ? "null" : ast1[2];
            let bind = [
                ["__dolist_list__", ast1[1]],
                ["__dolist_cnt__", ["length", ast1[1]]],
                ["__dolist_idx__", 0, ["+", "__dolist_idx__", 1]],
                [ast1[0], ["prop-get", "__dolist_list__", "__dolist_idx__"], ["prop-get", "__dolist_list__", "__dolist_idx__"]],
            ];
            let exit = [[">=", "__dolist_idx__", "__dolist_cnt__"], result_exp];
            ast = ["do*", bind, exit].concat(ast.slice(2));
            return compile_ast(ast);
        }
        case "if":
            return ("(" +
                compile_body1(ast[1]) +
                "?" +
                compile_body1(ast[2]) +
                ":" +
                compile_body(ast, 3) +
                ")");
        case "let":
        case "let*": {
            let ast1 = ast[1];
            let new_ast1 = [];
            for (let x of ast1) {
                if (typeof x === "string") {
                    new_ast1.push(x);
                    new_ast1.push(undefined);
                }
                else {
                    new_ast1.push(x[0]);
                    new_ast1.push(x[1]);
                }
            }
            return compile_ast(["_" + ast[0], new_ast1].concat(ast.slice(2)));
        }
        case "_let":
        case "_let*": {
            let vars = "(";
            let vals = "(";
            let assigns = "";
            for (let i = 0; i < ast[1].length; i++) {
                if (i % 2) {
                    if (i > 1)
                        vars += ",";
                    vars += ast[1][i - 1];
                    let val = compile_body1(ast[1][i]);
                    if (i > 1)
                        vals += ",";
                    vals += val;
                    assigns += ast[1][i - 1] + "=" + val + ";";
                }
            }
            vars += ")";
            vals += ")";
            if (ast[0] === "_let")
                return ("((function" +
                    vars +
                    "{return " +
                    compile_body(ast, 2) +
                    "})" +
                    vals +
                    ")");
            else
                return ("((function" +
                    vars +
                    "{" +
                    assigns +
                    "return " +
                    compile_body(ast, 2) +
                    "})())");
        }
        case "list": {
            let result = "[";
            for (let i = 1; i < ast.length; i++) {
                if (i > 1) result += ",";
                result += compile_body1(ast[i]);
            }
            result += "]";
            return result;
        }
        case "dict": {
            let result = "{";
            for (let i = 1; i < ast.length; i += 2) {
                if (i > 1) result += ",";
                result += compile_body1(ast[i]);
                result += ":";
                result += compile_body1(ast[i + 1]);
            }
            result += "}";
            return result;
        }
        case "set!":
            return compile_body1(ast[1]) + "=" + compile_body1(ast[2]);
        case "throw": {
            return "(function(){throw " + compile_body1(ast[1]) + "})()";
        }
        case "try": {
            let result = "(function(){try{return " + compile_body1(ast[1]) + "}catch(";
            if (ast[2][0] != "catch") throw "try without catch clause";
            result += ast[2][1] + "){return " + compile_body(ast[2], 2) + "}";
            result += "})()";
            return result;
        }
        case "until":
        case "while": {
            let condition = compile_body1(ast[1]);
            if (ast[0] === "until")
                condition = "!" + condition;
            return ("((function(){while(" +
                condition +
                "){" +
                compile_body(ast, 2) +
                "}})(),null)");
        }
        case "=":
            return "(" + compile_body1(ast[1]) + "===" + compile_body1(ast[2]) + ")";
        case "%":
        case "==":
        case "===":
        case "!=":
        case "!==":
        case "<":
        case ">":
        case "<=":
        case ">=":
            return "(" + compile_body1(ast[1]) + ast[0] + compile_body1(ast[2]) + ")";
        case "&&":
        case "||":
        case "&":
        case "|":
        case "+":
        case "-":
        case "*":
        case "**":
        case "/": {
            return "(" + insert_op(ast[0], ast.slice(1)) + ")";
        }
        default:
            let fcall = compile_body1(ast[0]) + "(";
            for (let i = 1; i < ast.length; i++) {
                if (i > 1)
                    fcall += ",";
                fcall += compile_body1(ast[i]);
            }
            fcall += ")";
            return fcall;
    }
}

function insert_op(op, rest) {
    if (rest.length === 1)
        return op + compile_body1(rest[0]);
    let result = [compile_body1(rest[0])];
    for (let i = 1; i < rest.length; i++) {
        result.push(op);
        result.push(compile_body1(rest[i]));
    }
    return result.join("");
}

function compile_do(ast) {
    let ast1 = ast[1];
    let parallel = ast[0] === "do";
    let ast1_len = ast1.length;
    let ast1_vars = [];
    if (parallel) {
        ast1_vars.push("__do__");
        //ast1_vars.push(["@", "new Array(" + ast1_len + ").fill(null)"]);
        ast1_vars.push("new Array(" + ast1_len + ").fill(null)");
    }
    ast1.forEach((x) => {
        ast1_vars.push(x[0]);
        ast1_vars.push(x[1]);
    });
    let ast2 = ast[2];
    if (ast2.length < 2)
        ast2 = [ast2[0], null];
    let until_ast = ["until", ast2[0]].concat(ast.slice(3));
    if (parallel) {
        ast1.forEach((x, i) => {
            if (x.length < 3)
                return;
            //let next_step = ["set!", ["@", "__do__[" + i + "]"], x[2]];
            let next_step = ["set!", "__do__[" + i + "]", x[2]];
            until_ast.push(next_step);
        });
        ast1.forEach((x, i) => {
            if (x.length < 3)
                return;
            //let next_step = ["set!", x[0], ["@", "__do__[" + i + "]"]];
            let next_step = ["set!", x[0], "__do__[" + i + "]"];
            until_ast.push(next_step);
        });
    }
    else {
        ast1.forEach((x) => {
            if (x.length < 3)
                return;
            let next_step = ["set!", x[0], x[2]];
            until_ast.push(next_step);
        });
    }
    let new_ast = [parallel ? "_let" : "_let*", ast1_vars].concat([until_ast]);
    new_ast.push(ast2[1]);
    return compile_ast(new_ast);
}

export function omljs() {
    let glob = {};
    glob.compile_ast_d = (ast) => glob.compile_ast(ast, true);
    glob.compile_ast = (ast, debug) => {
        if (debug)
            console.log(" [AST] " + JSON.stringify(ast));
        let text = compile_ast(ast);
        if (debug)
            console.log("[CODE] " + text);
        return text;
    };
    glob.compile_d = (text) => glob.compile(text, true);
    glob.compile = (text, debug) => {
        let steps = oml2ast(text);
        let result = "";
        for (let step of steps) {
            let exp = step[0];
            let ast = step[1];
            if (debug)
                console.log(" [OML] " + exp);
            if (debug)
                console.log(" [AST] " + JSON.stringify(ast));
            let text = compile_ast(ast);
            if (debug)
                console.log("[CODE] " + text);
            result += text + ";\n";
        }
        return result;
    };
    glob.run = (exp) => glob.exec(exp, true);
    glob.exec_d = (exp) => glob.exec(exp, true);
    glob.exec = (exp, debug) => {
        let src = exp;
        let steps = oml2ast(src);
        let last;
        let text = "";
        for (let step of steps) {
            let exp = step[0];
            let ast = step[1];
            var tm1 = new Date().getTime();
            try {
                if (debug)
                    console.log(" [OML] " + exp);
                if (debug)
                    console.log(" [AST] " + JSON.stringify(ast));
                text = compile_ast(ast);
                if (debug)
                    console.log("[CODE] " + text);
                let val = eval(text);
                last = val;
                let output;
                if (typeof val === "function") {
                    output = "function";
                }
                else if (!(val instanceof Array) &&
                    val instanceof Object &&
                    Object.prototype.toString.call(val) !== "[object Object]") {
                    try {
                        output =
                            Object.prototype.toString.call(val) + " " + JSON.stringify(val);
                    }
                    catch (e) { }
                }
                else {
                    try {
                        output = JSON.stringify(val);
                    }
                    catch (e) { }
                }
                var tm2 = new Date().getTime();
                if (debug) {
                    if (output === undefined) {
                        console.log("==> (" + (tm2 - tm1) + " ms)");
                        console.log(val);
                    }
                    else {
                        console.log("==> " + output + " (" + (tm2 - tm1) + " ms)");
                    }
                }
            }
            catch (e) {
                if (!debug)
                    console.log(" [OML] " + exp);
                if (!debug)
                    console.log(" [AST] " + JSON.stringify(ast));
                if (!debug)
                    console.log("[CODE] " + text);
                console.log("[EXCEPTION]");
                if (e.stack)
                    console.log(e.stack);
                else
                    console.log(e);
                throw e;
                break;
            }
        }
        return last;
    };
    glob.compileAll = (exp, debug) => {
        let src = exp;
        let steps = oml2ast(src);
        let result = "";
        for (let step of steps) {
            let exp = step[0];
            let ast = step[1];
            if (debug)
                console.log(" [OML] " + exp);
            if (debug)
                console.log(" [AST] " + JSON.stringify(ast));
            let text = compile_ast(ast);
            if (debug)
                console.log("[CODE] " + text);
            result += text + ";\n";
        }
        return result;
    };
    glob.execAll = (exp, debug) => {
        let text = glob.compileAll(exp, debug);
        try {
            return eval(text);
        } catch (e) {
            if (e.stack)
                console.log(e.stack);
            else
                console.log(e);
            throw e;
        }
    };
    glob.runAll = (exp) => {
        return glob.execAll(exp, true);
    };
    return glob;
}

export function run(exp) {
    let o = omljs();
    return o.run(exp);
}

export function runAll(exp) {
    let o = omljs();
    return o.runAll(exp);
}
