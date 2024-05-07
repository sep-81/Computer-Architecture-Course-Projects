module Register (
    par_in,
    clk,
    rst,
    load_en,
    par_out
);

  parameter word_len = 8;

  input [word_len - 1 : 0] par_in;
  input clk, rst, load_en;

  output reg [word_len - 1 : 0] par_out;

  always @(posedge clk) begin
    if (rst) par_out <= 'b0;
    else if (load_en) par_out <= par_in;
  end

endmodule
