import os
from flask import Flask, request, jsonify

app = Flask(__name__)

danh_sach_keys = {
    "ABC-123-ZIT": {"user": "anhA", "expire": "2025-12-31"},
    "DEF-456-ZIT": {"user": "anhB", "expire": "vĩnh viễn"}
}

@app.route("/verify", methods=["POST"])
def verify():
    data = request.get_json()
    key = data.get("key", "").strip()

    print(f"🔍 Nhận yêu cầu kiểm tra key: {key}")

    if key in danh_sach_keys:
        return jsonify({
            "status": "valid",
            "user": danh_sach_keys[key]["user"],
            "expire": danh_sach_keys[key]["expire"]
        })

    return jsonify({"status": "invalid"}), 400

# ✅ đoạn này thêm vào cuối file:
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 5000))  # Render sẽ set PORT
    app.run(host="0.0.0.0", port=port)
