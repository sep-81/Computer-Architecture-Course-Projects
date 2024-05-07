`include "inner_modules/Multiplier.v"
`include "inner_modules/Adder.v"
`include "inner_modules/Register.v"


module ErrorCheckerDP(
  b1_bus,
  b0_bus,
  x_bus,
  y_bus,
  h_x_ld,
  clk,
  rst,
  error_bus
);
  
  input [19:0] b1_bus, b0_bus, x_bus, y_bus;
  input h_x_ld, clk, rst;
  output [19:0] error_bus; 

  wire [39:0] mul1_out, adder1_out;
  wire [19:0] h_x_result;

  Multiplier #(.word_len(20)) mul1 (.in1(b1_bus), .in2(x_bus), .result(mul1_out));

  Adder #(.word_len(40)) adder1 (.in1(mul1_out), .in2({20'b0, b0_bus}), .cin(1'b0), .result(adder1_out));

  Adder #(.word_len(20)) adder2 (.in1(~h_x_result), .in2(y_bus), .cin(1'b1), .result(error_bus));

  Register #(.word_len(20)) h_x_reg (.par_in(adder1_out[29:10], .clk(clk), .rst(rst), .load_en(h_x_ld), .par_out(h_x_result));

endmodule
