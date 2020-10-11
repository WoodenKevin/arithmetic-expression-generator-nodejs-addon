const assert = require('assert');
const { generate } = require('bindings')('arithmetic-expression-generator');

let randTotal = Math.ceil(Math.random() * 90 + 10);

describe('# test generator', () => {
    res = generate(randTotal, 100);

    for (let i = 1; i <= randTotal; i++) {
        let { expression, answer } = res[i - 1];

        expression = expression.replace(/÷/g, '/');
        expression = expression.replace(/×/g, '*');

        let expressionEval = eval(expression).toFixed(4),
            answerEval = eval(answer).toFixed(4);

        it(`Round ${i}: ${expression} should equal to ${answer}`, () => {
            assert.strictEqual(expressionEval, answerEval);
        });
    }
});
