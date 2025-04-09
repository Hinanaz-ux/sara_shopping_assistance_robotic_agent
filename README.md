# sara_shopping_assistance_robotic_agent
This is a robotic agent that can traverse through the aisles of a supermarket using line following algorithm upon receiving a sorted list of item according to the stop number associated with each category of item, it collects the items using a pick and place arm and drops it into the cart attached.   
Each category of item is represented using an Aruco marker, the robot only stops at those markers which are specified in the initial list provided to it. 
Object detection is also used to detect the item , only when an item is detected the pick up sequence will be initiated.

i have uploaded 3 files , the arduino file contains the arduino code to control the movement of the robot , the pythoncode file contains the python code to manage the whole operation and the ui file contains a small gui code to select and sent the items to the robot.
