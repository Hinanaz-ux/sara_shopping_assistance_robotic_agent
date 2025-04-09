import cv2
import numpy as np
import serial
import time
import threading
from ultralytics import YOLO
import sara_ui

# Initialize serial communication with Arduino
try:
    arduino = serial.Serial('/dev/ttyUSB0', 9600)
    arduino.setDTR(False)
    time.sleep(2)
    arduino.flushInput()
    arduino.setDTR(True)
except Exception as e:
    print(f"Error: Unable to connect to Arduino: {e}")
    exit()

# Open the camera
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

if not cap.isOpened():
    print("Error: Unable to open the camera.")
    exit()

# Define the ArUco dictionary and detector parameters
dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_250)
parameters = cv2.aruco.DetectorParameters()
detector = cv2.aruco.ArucoDetector(dictionary, parameters)

# Load the YOLO model
model = YOLO('yolov8n.pt')
class_names_to_detect = ['apple', 'pizza', 'broccoli', 'clock','cup']
coco_classes = ['person', 'bicycle', 'car', 'motorbike', 'aeroplane', 'bus', 'train', 'truck', 'boat',
                'traffic light', 'fire hydrant', 'stop sign', 'parking meter', 'bench', 'bird', 'cat', 'dog',
                'horse', 'sheep', 'cow', 'elephant', 'bear', 'zebra', 'giraffe', 'backpack', 'umbrella',
                'handbag', 'tie', 'suitcase', 'frisbee', 'skis', 'snowboard', 'sports ball', 'kite',
                'baseball bat', 'baseball glove', 'skateboard', 'surfboard', 'tennis racket', 'bottle',
                'wine glass', 'cup', 'fork', 'knife', 'spoon', 'bowl', 'banana', 'apple', 'sandwich',
                'orange', 'broccoli', 'carrot', 'hot dog', 'pizza', 'donut', 'cake', 'chair', 'sofa',
                'pottedplant', 'bed', 'diningtable', 'toilet', 'tvmonitor', 'laptop', 'mouse', 'remote',
                'keyboard', 'cell phone', 'microwave', 'oven', 'toaster', 'sink', 'refrigerator', 'book',
                'clock', 'vase', 'scissors', 'teddy bear', 'hair drier', 'toothbrush']
class_indices = [coco_classes.index(name) for name in class_names_to_detect]

# Prompt the user to input marker IDs
try:
    target_markers = sara_ui.sorted_order
    if any(marker not in [0, 1, 2, 3] for marker in target_markers):
        raise ValueError("Invalid marker ID(s). Please enter numbers from 0 to 3.")
    print(f"Target markers: {target_markers}")
except ValueError as ve:
    print(f"Error: {ve}")
    exit()

robot_busy = False
detection_running = False

def detect_objects():
    global detection_running, robot_busy
    while detection_running:
        ret, frame = cap.read()
        if not ret:
            continue

        results = model.predict(source=frame, conf=0.5, classes=class_indices)
        if results and any(hasattr(r, 'boxes') and len(r.boxes.data) > 0 for r in results):
            print("Object detected! Sending OBDETECTED signal to Arduino.")
            arduino.write(b'OBDETECTED\n')
            detection_running = False
            robot_busy = False
            break

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Unable to read the frame. Restarting capture...")
            cap.release()
            time.sleep(1)
            cap = cv2.VideoCapture(0)
            continue

        markerCorners, markerIds, _ = detector.detectMarkers(frame)
        if markerIds is not None:
            detected_ids = markerIds.flatten()
            print(f"Detected marker IDs: {detected_ids}")
            for marker in target_markers:
                if marker in detected_ids and not robot_busy:
                    print(f"Target marker {marker} detected!")
                    arduino.write(b'TURN_SEQUENCE\n')
                    robot_busy = True
                    print("Sent TURN_SEQUENCE command to Arduino.")
                    target_markers.remove(marker)
                    break

        cv2.imshow("Frame", frame)

        if robot_busy and arduino.in_waiting > 0:
            try:
                response = arduino.readline().decode('utf-8', errors='ignore').strip()
                print(f"Response from Arduino: {response}")
                if response == "READY":
                    print("Arduino ready. Starting object detection...")
                    detection_running = True
                    detection_thread = threading.Thread(target=detect_objects)
                    detection_thread.start()
            except Exception as e:
                print(f"Error reading Arduino response: {e}")

        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("Exiting...")
            break

except KeyboardInterrupt:
    print("Interrupted by user.")

finally:
    print("Releasing resources...")
    cap.release()
    cv2.destroyAllWindows()
    arduino.close()
    print("Arduino connection closed.")
