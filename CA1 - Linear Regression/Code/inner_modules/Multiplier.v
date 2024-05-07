module Multiplier (
    in1,
    in2,
    result
);

  parameter word_len = 8;

  input [word_len - 1 : 0] in1, in2;
  output [word_len * 2 - 1 : 0] result;

  assign result = in1 * in2;

endmodule
