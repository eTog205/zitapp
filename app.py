from flask import Flask, request, jsonify

app = Flask(__name__)

# danh sách key hợp lệ – sau này có thể đọc từ file hoặc database
danh_sach_keys = {
    "ABC-123-ZIT": {"user": "anhA", "expire": "2025-12-31"},
    "DEF-456-ZIT": {"user": "anhB", "expire": "vĩnh viễn"}
}

@app.route("/verify", methods=["POST"])
def verify():
    data = request.get_json()
    key = data.get("key", "").strip()

    if key in danh_sach_keys:
        return jsonify({
            "status": "valid",
            "user": danh_sach_keys[key]["user"],
            "expire": danh_sach_keys[key]["expire"]
        })

    return jsonify({"status": "invalid"}), 400

# nếu chạy local để test
if __name__ == "__main__":
    app.run(debug=True)
