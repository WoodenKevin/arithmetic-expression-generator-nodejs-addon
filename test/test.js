const assert = require('assert');
const { generate } = require('bindings')('arithmetic-expression-generator');

let randTotal = Math.ceil(Math.random() * 900 + 10);

describe('# test generator', () => {
    try {
        res = generate(randTotal, 1000);

        for (let i = 1; i <= randTotal; i++) {
            let { expression, answer } = res[i - 1];

            let formattedExpression = expression
                .replace(/÷/g, '/')
                .replace(/×/g, '*');

            let expressionEval = eval(formattedExpression).toFixed(4),
                answerEval = eval(answer).toFixed(4);

            it(`Round ${i}: ${expression} should equal to ${answer}`, () => {
                assert.strictEqual(expressionEval, answerEval);
            });
        }
    } catch (err) {
        console.log(err);
    }
});
