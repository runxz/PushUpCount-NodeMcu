import cv2
import imutils
import mediapipe as mp
import socket

mp_draw = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

nodeMCU_ip = ""  # Replace with the IP address of your NodeMCU
nodeMCU_port = 5005

count = 0
position = None

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

cap = cv2.VideoCapture("")# Replace with the IP address of your Inpur CAM

with mp_pose.Pose(min_detection_confidence=0.7, min_tracking_confidence=0.7) as pose:
    while cap.isOpened():
        success, image = cap.read()

        if not success:
            print("Empty camera")
            break

        image = imutils.resize(image, width=500)
        image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
        result = pose.process(image)
        lmList = []

        if result.pose_landmarks:
            mp_draw.draw_landmarks(image, result.pose_landmarks, mp_pose.POSE_CONNECTIONS,
                                   landmark_drawing_spec=mp_draw.DrawingSpec(color=(0, 255, 0), thickness=2,
                                                                             circle_radius=2))

            for id, im in enumerate(result.pose_landmarks.landmark):
                h, w, _ = image.shape
                X, Y = int(im.x * w), int(im.y * h)
                lmList.append([id, X, Y])

            if len(lmList) != 0:
                if lmList[12][2] >= lmList[14][2] and lmList[11][2] >= lmList[13][2]:
                    position = "down"

                if lmList[12][2] <= lmList[14][2] and lmList[11][2] <= lmList[13][2] and position == "down":
                    position = "up"
                    count += 1

        print(count)
        cv2.imshow("Push-up Counter", cv2.flip(image, 1))
        key = cv2.waitKey(1)

        # Send the push-up count as a UDP message
        message = str(count).encode()
        sock.sendto(message, (nodeMCU_ip, nodeMCU_port))

        # Program terminates when 'q' is pressed
        if key == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()

# Close the socket
sock.close()
