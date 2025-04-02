#include "hieuung.h"
#include <imgui_internal.h>

bool hieu_ung_mo_len(float thoi_gian)
{
	// lấy tên cửa sổ hiện tại
	const char* ten_id = ImGui::GetCurrentWindow()->Name;
	static std::unordered_map<std::string, float> timer_map;

	ImGuiIO& io = ImGui::GetIO();
	float& thoi_gian_da_chay = timer_map[ten_id];

	// cập nhật thời gian chạy
	thoi_gian_da_chay += io.DeltaTime;

	// tính alpha từ 0.0 đến 1.0
	float ti_le = std::min(thoi_gian_da_chay / thoi_gian, 1.0f);

	// lưu lại alpha gốc để khôi phục sau
	float alpha_goc = ImGui::GetStyle().Alpha;

	// set alpha tạm thời cho cửa sổ này
	ImGui::GetStyle().Alpha = alpha_goc * ti_le;

	// tự khôi phục lại alpha sau frame này
	if (ti_le >= 1.0f)
	{
		ImGui::GetStyle().Alpha = alpha_goc;
		timer_map.erase(ten_id);
	}

	return (ti_le >= 1.0f);
}
bool hieu_ung_mo_len_dungten(const char* ten_cuaso, float thoigian)
{
	static std::unordered_map<std::string, float> timer_map;
	ImGuiIO& io = ImGui::GetIO();
	float& tg = timer_map[ten_cuaso];
	tg += io.DeltaTime;

	float tile = std::min(tg / thoigian, 1.0f);

	// Đẩy Alpha tạm thời lên stack, đảm bảo các cửa sổ khác không bị ảnh hưởng
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tile);

	// Trả về true nếu hoàn tất hiệu ứng, false nếu còn đang fade-in
	bool xong = tile >= 1.0f;
	if (xong)
		timer_map.erase(ten_cuaso);

	return xong;
}
