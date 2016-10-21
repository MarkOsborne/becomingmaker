inch = 25.4; // inch in mm, use to define dimensions in inches

/*
 * Use these variables to control which parts to draw
 */
draw_onbed=1;    // draw all part flat on bed ready to print
draw_board=0;    // draw sample board (off to print)
draw_base=1;     // draw base
draw_top=1;      // draw top
draw_plungers=1; //draw plungers

/*
 * The following parameters control the dimensions of the enclosure
 */
 
// thickness of top, bottom, side and lip walls, lid walls 2x
wall_thickness = 1.4;

// lip depth and pullback from corners
lipSize=4+wall_thickness;
lip_pullback=8;

// gap below bottom of board
bottom_gap = 3;

// Dimensions from eagle brd
board = [1.65*inch,2*inch,1.6]; // Board dimensions
drillSize = 0.11023622 * inch;  // Mounting hole diameter
holeOffset = 0.1 * inch;        // Mounting hole center to board corner offset

// Nudged button 0.5mm so button guide would clear XBee
upButton = [0.65*inch-0.5,1.265*inch];  // Up button board position and height
downButton = [0.65*inch-0.5,0.81*inch]; // Down button board position and height

// Gap between board and walls 
leftGap=2;
frontGap=2;
rightGap=2+6.5; // 6.5mm Xbee overhang
backGap=2;

poleTop=2;             // Amount support poles protrude above board
maxComponentHeight=12; // Height of tallest component on board
componentClearance=1;  // Clearence above tallest component

// Dimensions of buttons on board (actual 6mm x 6mm)
buttonSize = [6, 6, 5];

// Plunger is user visible button that presses on button component
plungerHeight = 4; // Height plunger protrudes above case

// base wall height above board - requires space for lip and cutout gap
wall_height = 0.5*inch;

// cutout position relative to board
cutout_position = [ 5, 3 ]; // x and z above board
cutout_size = [ 9.6, 4 ];   // width and height
ir_position = [ 16.5, 0 ];  // x and z above board
ir_size = [ 6.5, 2 ];       // width and height

/*
 * Play variables to control tightness between surfaces
 */

plungerPlay = 0.4;        // Play around plunger, loose enough to slide freely 
plunger_top_play = 0.8;   // Play around pluger top (user button), very touchy when shape is triangle
boardHoleSlack = inch/64; // Slack between mounting holes and support poles
cutoutplug_slack = 0.8;   // Plug extends from top into cutout to shorten height
lipPlay=0.2;              // Play between lip and case - tight enough for friction fit
guide_gap=1;              // Gap between bottom of guide and top of button component
guide_wall_thickness=0.5; // Thickness of pluger guide wall
    
/*
 * Calculated variables
 */

// Perfectly aligned surfaces don't always diff correctly in OpenScad
dd=0.1; // dd (diff delta) add to diff dimension to ensure complete cut

// Mounting hole positions relative to board
boardBL = [holeOffset,holeOffset];                       // Bottom left 
boardBR = [board[0],0]+[-holeOffset,holeOffset];         // Bottom right
boardTL = [0,board[1]]+[holeOffset,-holeOffset];         // Top left
boardTR = [board[0],board[1]]+[-holeOffset,-holeOffset]; // Top right
boardHoleRadius = drillSize/2;

// Base dimensions
base = [ board[0] + leftGap+rightGap+wall_thickness*2,
         board[1] + frontGap+backGap+wall_thickness*2,
         board[2] + wall_thickness + bottom_gap + wall_height];

// Height of inside of lid
topHeight = wall_thickness + bottom_gap + board[2] +
            maxComponentHeight + componentClearance;

// Cutouts in base for 12V wires (y will be mirrored)
cutoutPos = [ wall_thickness + leftGap + cutout_position[0],
              0,
              bottom_gap + wall_thickness + board[2] + cutout_position[1] ];
cutoutSize = [ cutout_size[0], wall_thickness*2, base[2]-cutoutPos[2] ];

// cutout in base for IR receiver
irPos = [ wall_thickness + leftGap + ir_position[0],
          0,
          wall_thickness + bottom_gap + board[2] + ir_position[1]];
irSize = [ ir_size[0], wall_thickness*2, base[2]-irPos[2] ];

// Position of buttons
upButtonPos = [upButton[0]-buttonSize[0]/2, upButton[1]-buttonSize[0]/2, board[2]];
downButtonPos = [downButton[0]-buttonSize[0]/2, downButton[1]-buttonSize[0]/2, board[2]];

// Guide is a button sized shaft from the top of the button to the roof
guideSize=[buttonSize[0],buttonSize[1],topHeight-(bottom_gap+wall_thickness+upButtonPos[2]+buttonSize[2])];

upGuidePos=[leftGap+wall_thickness+upButtonPos[0], 
           frontGap+wall_thickness+upButtonPos[1],
           bottom_gap+wall_thickness+upButtonPos[2]+buttonSize[2]];

downGuidePos=[leftGap+wall_thickness+downButtonPos[0], 
           frontGap+wall_thickness+downButtonPos[1],
           bottom_gap+wall_thickness+downButtonPos[2]+buttonSize[2]];

/*
 * Modules
 */
 
// Plunger shapes: 0=circle, 1=up, 2=down
module drawPluger(guidePos, buttonPos, shape)
{
  translate(guidePos+[plungerPlay ,plungerPlay ,plungerPlay ])
    cube(guideSize-[plungerPlay*2,plungerPlay*2,plungerPlay *2]);

  translate([leftGap+wall_thickness,frontGap+wall_thickness,topHeight-plungerPlay])
    translate([buttonPos[0]+buttonSize[0]/2,buttonPos[1]+buttonSize[0]/2,0])
    {
      if(shape==0)
        cylinder(d=buttonSize[0]-plunger_top_play*2,h=wall_thickness+plungerPlay+plungerHeight,$fn=24);
      if(shape==1)
          rotate([0,0,90]) cylinder(d=buttonSize[0]-plunger_top_play*2,h=wall_thickness+plungerPlay+plungerHeight,$fn=3);
      if(shape==2)
          rotate([0,0,-90]) cylinder(d=buttonSize[0]-plunger_top_play*2,h=wall_thickness+plungerPlay+plungerHeight,$fn=3);
    }
}

// Enclosure top (lid)
module drawTop()
{
  difference()
  {
    translate([0,0,base[2]])
     cube([base[0],base[1],topHeight-base[2]+wall_thickness]);
    // leave room for wall_thickness lip
    translate([wall_thickness*2,wall_thickness*2,base[2]-dd])
     cube([base[0]-wall_thickness*4,base[1]-wall_thickness*4,topHeight-base[2]+dd]);
    translate([leftGap+wall_thickness,frontGap+wall_thickness,topHeight])
    {
      // button holes
      translate([upButtonPos[0]+buttonSize[0]/2,upButtonPos[1]+buttonSize[0]/2,-dd])
        rotate([0,0,90]) cylinder(d=buttonSize[0],h=wall_thickness+dd*2,$fn=3);
      translate([downButtonPos[0]+buttonSize[0]/2,downButtonPos[1]+buttonSize[0]/2,-dd])
        rotate([0,0,-90]) cylinder(d=buttonSize[0],h=wall_thickness+dd*2,$fn=3);
    }
  }

  difference()
  {
    translate([wall_thickness+lipPlay,wall_thickness+lipPlay,base[2]-lipSize])
     bottomBevelCube([base[0]-wall_thickness*2-lipPlay*2,base[1]-wall_thickness*2-lipPlay*2,lipSize],wall_thickness);
  
    
    // pull back from corners      translate([wall_thickness+lipPlay,wall_thickness+lipPlay,base[2]-lipSize])
    translate([wall_thickness+lipPlay,wall_thickness+lipPlay,base[2]-lipSize])
      cube([lip_pullback-wall_thickness*2-lipPlay*2,lip_pullback-wall_thickness*2-lipPlay*2,lipSize]);
    translate([base[0]-lip_pullback+wall_thickness+lipPlay,wall_thickness+lipPlay,base[2]-lipSize])
      cube([lip_pullback-wall_thickness*2-lipPlay*2,lip_pullback-wall_thickness*2-lipPlay*2,lipSize]);
    translate([base[0]-lip_pullback+wall_thickness+lipPlay,base[1]-lip_pullback+wall_thickness+lipPlay,base[2]-lipSize])
      cube([lip_pullback-wall_thickness*2-lipPlay*2,dd+lip_pullback-wall_thickness*2-lipPlay*2,lipSize]);
    translate([wall_thickness+lipPlay,base[1]-lip_pullback+wall_thickness+lipPlay,base[2]-lipSize])
      cube([lip_pullback-wall_thickness*2-lipPlay*2,dd+lip_pullback-wall_thickness*2-lipPlay*2,lipSize]);

    translate([wall_thickness*2,wall_thickness*2,base[2]-lipSize-dd])
     cube([base[0]-wall_thickness*4,base[1]-wall_thickness*4,lipSize+dd*2]);
    
  }

  // Cutout plugs
  translate(cutoutPos+[cutoutplug_slack,0,cutout_size[1]])
     cube(cutoutSize-[cutoutplug_slack*2 ,0,cutout_size[1]]);
  translate(cutoutPos+[cutoutplug_slack,
            wall_thickness*2+frontGap+backGap+board[1]-cutoutSize[1],
            cutout_size[1]])
    cube(cutoutSize-[cutoutplug_slack*2,0,cutout_size[1]]);
  translate(irPos+[cutoutplug_slack,
            wall_thickness*2+frontGap+board[1]+backGap-irSize[1],
            ir_size[1]])
    cube(irSize-[cutoutplug_slack*2,0,ir_size[1]]);

  
  // Guides
  difference()
  {
    // Join guides together to improve strength
    hull()
    {
      translate(upGuidePos+[-guide_wall_thickness,-  guide_wall_thickness,guide_gap])
          cube(guideSize+[guide_wall_thickness*2,  guide_wall_thickness*2,-guide_gap]);
      translate(downGuidePos+[-guide_wall_thickness,-guide_wall_thickness,1])
          cube(guideSize+[guide_wall_thickness*2,guide_wall_thickness*2,-guide_gap]);
    }
    translate(upGuidePos+[0,0,dd])
        cube(guideSize+[0,0,dd]);
    translate(downGuidePos+[0,0,dd])
        cube(guideSize+[0,0,dd]);
  }

  // Support to hold board down
  drawSupport();

}

// Supports extend from top to hold the board down
// as an alternative to using screws
module drawSupport()
{
  supportPlay = 0.2;
  supportZ = bottom_gap+wall_thickness+board[2]+supportPlay;
  supportHeight = topHeight-supportZ;
  supportThickness = 0.8;
  translate([leftGap+wall_thickness,frontGap+wall_thickness,supportZ])
  {
      drawSupportPost(boardBL, supportHeight, supportThickness, supportPlay);
      drawSupportPost(boardBR, supportHeight, supportThickness, supportPlay);
      drawSupportPost(boardTL, supportHeight, supportThickness, supportPlay);
      drawSupportPost(boardTR, supportHeight, supportThickness, supportPlay);
  }
}

// Individual support post
module drawSupportPost(position, supportHeight, supportThickness, supportPlay)
{
        difference()
        {
         translate([position[0],position[1],supportHeight/2])
          cylinder(h=supportHeight, r=boardHoleRadius+supportThickness, center=true, $fn=24);
         translate([position[0],position[1],(poleTop+supportPlay)/2 - 0.1/2])
          cylinder(h=poleTop+supportPlay+0.1, r=boardHoleRadius, center=true, $fn=24);
        }
}

// Enclosure Base
module drawBase()
{
  difference()
  {
     cube([base[0],base[1],base[2]]);
     translate([wall_thickness,wall_thickness,wall_thickness])
       cube([base[0]-wall_thickness*2, base[1]-wall_thickness*2, base[2]-wall_thickness+dd]);
     translate(cutoutPos)
       cube(cutoutSize);
     translate(cutoutPos+[0,wall_thickness+frontGap+backGap+board[1],0])
       cube(cutoutSize);
     translate(irPos+[0,wall_thickness+frontGap+backGap+board[1],0])
       cube(irSize);

  }

  drawPost(boardBL);   
  drawPost(boardBR);   
  drawPost(boardTL);   
  drawPost(boardTR);
   
}

// Base post to hold board
module drawPost(position)
{
  poleHeight=bottom_gap+board[2]+poleTop;
  poleBase=bottom_gap;
  translate([leftGap+wall_thickness,frontGap+wall_thickness,wall_thickness])
  {
    translate(position)
    {
      translate([0,0,poleHeight/2])
        cylinder(h=poleHeight, r=boardHoleRadius-boardHoleSlack, center=true, $fn=24);
      translate([0,0,poleBase/2])
        cylinder(h=poleBase, r=boardHoleRadius+1, center=true, $fn=24);
    }
  }
}

// Simplified board for visualizing fit in OpenScad
module drawBoard ()
{
  translate([leftGap+wall_thickness,frontGap+wall_thickness,bottom_gap+wall_thickness])
  {
    difference()
    {
      cube(board);
      translate(boardBL)
        cylinder(h=20, r=boardHoleRadius, center=true, $fn=24);
      translate(boardBR)
        cylinder(h=20, r=boardHoleRadius, center=true, $fn=24);
      translate(boardTL)
        cylinder(h=20, r=boardHoleRadius, center=true, $fn=24);
      translate(boardTR)
        cylinder(h=20, r=boardHoleRadius, center=true, $fn=24);
    }
    translate(upButtonPos) cube(buttonSize-[0,0,1]);
    translate([upButton[0], upButton[1], board[2]])
      cylinder(d=3.5,h=buttonSize[2], $fn=24);
  
    translate(downButtonPos) cube(buttonSize-[0,0,1]);
    translate([downButton[0], downButton[1], board[2]])
       cylinder(d=3.5,h=buttonSize[2], $fn=24);
  }
}

// Draw a cube or shape "s" which a bevel of size "bevel" on top
module topBevelCube(s, bevel)
{
    b = bevel;
    polyhedron(
        points = [ 
            [0,0,0],[s[0],0,0],[0,s[1],0],[s[0],s[1],0],
            [b,b,s[2]],[s[0]-b,b,s[2]],[b,s[1]-b,s[2]],[s[0]-b,s[1]-b,s[2]],
            [0,0,s[2]-b],[s[0],0,s[2]-b],[0,s[1],s[2]-b],[s[0],s[1],s[2]-b]],
        faces = [ 
            [0,1,3], [0,3,2],    // Bottom
            [4,7,5], [4,6,7],    // Top
            [0,8,1], [1,8,9],    // Front
            [1,9,3], [3,9,11],   // Right
            [2,3,11], [2,11,10], // Back, 
            [2,10,0], [0,10,8],  // Left
            [4,5,8], [5,9,8],    // Front Bevel
            [5,7,9], [7,11,9],   // Right Bevel
            [7,6,11], [11,6,10], // Back Bevel
            [4,10,6], [4,8,10]   // Left Bevel 
    ]
    );
}

// Draw a cube or shape "s" which a bevel of size "bevel" on bottom
// We use this to make the top lip easier to insert into the base
module bottomBevelCube(s, bevel)
{  
    translate([0,s[1],s[2]])
    rotate([180,0,0])
    topBevelCube([s[0],s[1],s[2]],bevel);
}


/*
 * Code to draw parts
 * Use draw_* to control which parts are rendered
 */

if (draw_board) drawBoard();
if (draw_base) drawBase();
if (draw_top)
{
  if (draw_onbed)
    translate([-10,0,topHeight+wall_thickness])
      rotate([0,180,0])
        drawTop();
  else
    drawTop();
}
if (draw_plungers)
{
  if (draw_onbed)
  {
    translate(-[upGuidePos[0],0,upGuidePos[2]] - [8,0,plungerPlay])
      drawPluger(upGuidePos, upButtonPos, 1);
    translate(-[downGuidePos[0],0,downGuidePos[2]] - [8,0,plungerPlay])
      drawPluger(downGuidePos, downButtonPos, 2);
  }
  else
  {
    drawPluger(upGuidePos, upButtonPos, 1);
    drawPluger(downGuidePos, downButtonPos, 2);
  }
}
