module Divide (
    dividend,
    divisor,
    result
);

  parameter word_len = 8;

  input [word_len - 1 : 0] dividend, divisor;
  output [word_len - 1 : 0] result;
  assign result = dividend / divisor;

endmodule
