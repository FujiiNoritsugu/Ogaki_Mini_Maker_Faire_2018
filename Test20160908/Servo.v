module Servo(
    clkin,
    rstn,
    cntin,
    pwmout
    );
    
    input clkin;
     input rstn;
     input [7:0]cntin;
     output reg pwmout;
     reg [12:0]counter;
	  reg  [12:0] width;
	  wire [12:0] width_wire = cntin;
	  //reg [12:0] width_reg;
	  //assign width = width_wire + 7'd75;
	  //assign width_wire = width_reg;
	  
	  //reg [7:0] cntin_intern;
	  //wire [7:0] cntin_wire = cntin;
	  //assign cntin_wire = cntin_intern;
	  
    always@(posedge clkin or posedge rstn)
	 begin
	 if (rstn == 1'b1) begin
             counter <= 13'd0;
             pwmout <= 1'b0;
    end
    else begin
	      //width <= 9'd300;
			if (counter > 13'd1499) begin
                 counter <= 13'd0;
                 pwmout <= 1'b0;
         end
         else if (counter < cntin)
                 pwmout <= 1'b1;
         else
                 pwmout <= 1'b0;
         counter <= counter + 1'b1;
    end
    end

endmodule