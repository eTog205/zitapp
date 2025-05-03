// giaodien.cpp
#include "giaodien.h"
#include "chay_luongphu.h"
#include "chucnang_cotloi.h"
#include "logic_giaodien.h"
#include "net.h"

#include <imgui_stdlib.h>

std::string tab_hientai = "bangdl";
giaodien gd;

namespace
{
	bool sosanh_ngay(const std::string& ngay_yyyy_mm_dd)
	{
		std::tm tm = {};
		std::istringstream ss(ngay_yyyy_mm_dd);
		ss >> std::get_time(&tm, "%Y-%m-%d");
		if (ss.fail())
			return false;

		// Chuyển sang time_t
		std::time_t ngay_key = std::mktime(&tm);
		if (ngay_key == -1)
			return false;

		// Lấy thời gian hiện tại
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		// So sánh
		return now <= ngay_key;
	}
} // namespace

void hienthi_loi(const std::string& loi, const demtg::time_point tg_loi, const int tg_tb_mat)
{
	if (!loi.empty())
	{
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(demtg::now() - tg_loi).count();
		if (elapsed < tg_tb_mat)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", loi.c_str());
		}
	}
}

// Hàm tính toán vị trí, kích thước, tên cửa sổ và cờ cho một cửa sổ con
thongtin_cuaso_imgui tinh_thongtin_cuaso(const int chieurong_manhinh, const int chieucao_manhinh)
{
	thongtin_cuaso_imgui tt;

	// Tính toạ độ
	tt.vitri.x = gd.chieurong_menuben + gd.letrai_menuben * 2;
	tt.vitri.y = gd.letren_bang;

	// Tính kích thước
	tt.kichthuoc.x = static_cast<float>(chieurong_manhinh) - gd.letrai_menuben * 3 - gd.chieurong_menuben;
	tt.kichthuoc.y = static_cast<float>(chieucao_manhinh) - gd.letren_bang;

	return tt;
}

void capnhat_bang_phanmem()
{
	std::vector<column_config> visible_columns;
	for (const auto& col : lg_gd.ch_b.columns)
	{
		if (col.hienthi)
			visible_columns.push_back(col);
	}

	const int tong_cot = static_cast<int>(visible_columns.size());
	if (tong_cot == 0)
		return;

	if (ImGui::BeginTable("BangPhanMem", tong_cot, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable))
	{
		for (const auto& cot : visible_columns)
		{
			int co = 0;
			if (!cot.thaydoi_kt)
				co |= ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize;
			if (!cot.sapxep)
			{
				co |= ImGuiTableColumnFlags_NoSort;
			}

			ImGui::TableSetupColumn(cot.tieude.c_str(), co, cot.chieurong_hientai);
		}

		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for (int i = 0; i < tong_cot; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::PushID(i);
			ImGui::TableHeader(visible_columns[i].tieude.c_str());

			// Nếu chuột phải vào tiêu đề, mở menu
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Ẩn cột này"))
				{
					visible_columns[i].hienthi = false;
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		// Xử lý sắp xếp nếu cần
		if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
		{
			if (sort_specs->SpecsDirty && sort_specs->SpecsCount > 0)
			{
				const ImGuiTableColumnSortSpecs* spec = &sort_specs->Specs[0];
				int sorted_column = spec->ColumnIndex;
				// Kiểm tra cột sắp xếp có hợp lệ và không phải cột "chon"
				if (sorted_column >= 0 && sorted_column < static_cast<int>(visible_columns.size()))
				{
					// Nếu cột này không cho phép sắp xếp thì bỏ qua xử lý sắp xếp
					if (visible_columns[sorted_column].sapxep)
					{
						const std::string& col_id = visible_columns[sorted_column].id;
						int data_index = -1;
						if (col_id == "id")
							data_index = 0;
						else if (col_id == "ten")
							data_index = 1;
						else if (col_id == "phanloai")
							data_index = 2;
						// Nếu cột là cột dữ liệu hợp lệ, tiến hành sắp xếp
						if (data_index >= 0)
						{
							bool ascending = (spec->SortDirection == ImGuiSortDirection_Ascending);
							std::ranges::sort(lg_gd.data,
											  [data_index, ascending](const std::vector<std::string>& a, const std::vector<std::string>& b)
											  {
												  if (ascending)
													  return a[data_index] < b[data_index];
												  return a[data_index] > b[data_index];
											  });
						}
					}
				}
				sort_specs->SpecsDirty = false;
			}
		}

		float rowHeight = 20.0f;
		// hiển thị dữ liệu cột
		for (size_t hang = 0; hang < lg_gd.data.size(); ++hang)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
			bool row_overlay_clicked = false;

			for (int vitri_cot = 0; vitri_cot < tong_cot; vitri_cot++)
			{
				ImGui::TableSetColumnIndex(vitri_cot);
				ImVec2 cellPos = ImGui::GetCursorScreenPos();
				const auto& cot = visible_columns[vitri_cot];
				if (cot.id == "chon")
				{
					std::string id = lg_gd.data[hang][0];
					ImGui::Checkbox(("##check" + std::to_string(hang)).c_str(), &lg_gd.selected_map[id]);
				}
				else
				{
					int vitri_dulieu = 0;
					if (cot.id == "id")
						vitri_dulieu = 0;
					else if (cot.id == "ten")
						vitri_dulieu = 1;
					else if (cot.id == "phanloai")
						vitri_dulieu = 2;
					if (vitri_dulieu < static_cast<int>(lg_gd.data[hang].size()))
					{
						// ImGui::Text("%s", lg_gd.data[hang][vitri_dulieu].c_str());
						ImGui::TextUnformatted(lg_gd.data[hang][vitri_dulieu].c_str());
					}
				}
				if (cot.id != "chon")
				{
					float columnWidth = ImGui::GetColumnWidth();
					ImGui::SetCursorScreenPos(cellPos);

					if (ImGui::InvisibleButton(("##overlay_" + std::to_string(hang) + "_" + std::to_string(vitri_cot)).c_str(), ImVec2(columnWidth, rowHeight)))
					{
						// Nếu bấm vào bất kỳ ô nào (trừ ô checkbox), ta đánh dấu
						row_overlay_clicked = true;
					}
				}
			}
			if (row_overlay_clicked)
			{
				std::string id = lg_gd.data[hang][0];
				lg_gd.selected_map[id] = !lg_gd.selected_map[id];
			}
		}
		ImGui::EndTable();
	}
}

void giaodien_thanhcongcu(const int chieurong_manhinh, const int chieucao_manhinh)
{
	const float chieurong_hientai = static_cast<float>(chieurong_manhinh) - gd.letrai_menuben * 2;
	const float chieucao_hientai = static_cast<float>(chieucao_manhinh) - gd.chieucao_menuben - gd.letrai_menuben * 2;

	ImGui::SetNextWindowPos(ImVec2(gd.letrai_menuben, gd.letrai_menuben));
	ImGui::SetNextWindowSize(ImVec2(chieurong_hientai, chieucao_hientai));

	ImGui::Begin("thanh công cụ", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	if (ImGui::Button("nút cài đặt"))
	{
		for (auto& item : lg_gd.selected_map)
		{
			if (item.second)
				logic_giaodien::chaylenh_winget(item.first);
		}
	}

	giaodien_cuasotuychinh_menunho();

	ImGui::End();
}

void giaodien_cuasotuychinh_menunho()
{
	static ImVec2 toggleButtonMin, toggleButtonMax;

	if (ImGui::Button("Mở Menu"))
	{
		gd.isMenuOpen = !gd.isMenuOpen;
	}

	toggleButtonMin = ImGui::GetItemRectMin();
	toggleButtonMax = ImGui::GetItemRectMax();

	if (gd.isMenuOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(250, 200));
		ImGui::SetNextWindowBgAlpha(0.9f);
		ImGui::Begin("Menu", &gd.isMenuOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

		ImGui::Text("Tùy chỉnh hiển thị cột:");
		ImGui::Separator();

		// Cho phép ẩn/hiện tất cả các cột (ngoại trừ cột "chon")
		for (auto& column : lg_gd.ch_b.columns)
		{
			if (column.id != "chon")
			{
				ImGui::Checkbox(column.tieude.c_str(), &column.hienthi);
			}
		}

		// Lấy vị trí và kích thước của cửa sổ menu trước khi kết thúc cửa sổ
		ImVec2 menuPos = ImGui::GetWindowPos();
		ImVec2 menuSize = ImGui::GetWindowSize();
		ImGui::End();

		// ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		//// Màu khung xanh lá, độ dày 2.0f
		// draw_list->AddRect(menuPos, ImVec2(menuPos.x + menuSize.x, menuPos.y + menuSize.y),
		//				   IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);

		// Kiểm tra click chuột trái bên ngoài vùng menu để đóng menu
		if (ImGui::IsMouseClicked(0))
		{
			if (!ImGui::IsMouseHoveringRect(toggleButtonMin, toggleButtonMax))
			{
				ImVec2 mousePos = ImGui::GetIO().MousePos;
				if (mousePos.x < menuPos.x || mousePos.x > menuPos.x + menuSize.x || mousePos.y < menuPos.y || mousePos.y > menuPos.y + menuSize.y)
				{
					gd.isMenuOpen = false;
				}
			}
		}
	}
}

void giaodien_menuben(const int chieucao_manhinh)
{
	constexpr float le_nut = 10.0f; // indent từ mép content
	constexpr float chieucao_nut = 30.0f; // chiều cao cố định của nút

	// 1) Tính chiều cao + chiều rộng hiện tại của menu
	gd.chieucao_menuben = static_cast<float>(chieucao_manhinh) - gd.letren_menuben;
	const float chieurong_hientai = tt_thugonkichthuoc(gd.menuben_thugon, gd.yeucau_thugon, gd.batdau_thugon, gd.chieurong_menuben_morong, gd.chieurong_menuben_thugon, gd.thoigian_thugon);
	gd.chieurong_menuben = chieurong_hientai;

	// 2) Bắt đầu window
	ImGui::SetNextWindowPos(ImVec2(gd.letrai_menuben, gd.letren_menuben));
	ImGui::SetNextWindowSize(ImVec2(chieurong_hientai, gd.chieucao_menuben));
	ImGui::Begin("Menu bên", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	// 3) Khoảng cách nhỏ phía trên
	ImGui::Dummy(ImVec2(0.0f, 6.0f));

	// 4) Kích thước chung cho tất cả nút
	ImVec2 buttonSize(chieurong_hientai - 2 * le_nut, chieucao_nut);

	// ==== Nút thu gọn ====
	ImGui::SetCursorPosX(le_nut);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f)); // trung tâm
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.92f, 0.92f, 0.92f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.78f, 0.88f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.65f, 0.85f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	if (ImGui::Button(gd.menuben_thugon ? ">" : "<", buttonSize))
	{
		gd.yeucau_thugon = true;
		gd.batdau_thugon = std::chrono::steady_clock::now();
	}
	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(); // pop ButtonTextAlign
	ImGui::PopStyleVar(); // pop FrameRounding

	// ==== Các nút menu chính ====
	static const std::vector<MenuItem> menu_items = { { L"Bảng dữ liệu", "bangdl" }, { L"Tiện ích", "tienich" }, { L"Cài đặt", "caidat" }, { L"Hỗ trợ", "hotro" } };

	for (auto& item : menu_items)
	{
		bool selected = (item.id == tab_hientai);

		ImGui::SetCursorPosX(le_nut);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f)); // trái + giữa dọc

		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, selected ? ImVec4(0.85f, 0.85f, 0.85f, 1.0f) : ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selected ? ImVec4(0.75f, 0.75f, 0.75f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, selected ? ImVec4(0.70f, 0.70f, 0.70f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 0.4f));

		// Tạo label hiển thị + ###ID để ImGui giữ ID riêng
		std::string display = tt_vanbancothenhinthay(item.toanbo_vanban, chieurong_hientai, gd.chieurong_menuben_thugon, gd.chieurong_menuben_morong);
		std::string label = display + "###" + item.id;

		ImGui::SetCursorPosX(le_nut);
		if (ImGui::Button(label.c_str(), buttonSize))
		{
			tab_hientai = item.id;
			ImGui::SetWindowFocus(item.id.c_str());
		}

		// ==== Vẽ indicator khi selected ====
		if (selected)
		{
			ImDrawList* drawlist = ImGui::GetWindowDrawList();
			ImVec2 pmin = ImGui::GetItemRectMin();

			// Thông số indicator
			const float w = 4.0f; // độ rộng cố định
			const float stripeH = 2.0f; // chỉ 2px ở đầu và 2px ở đuôi
			const float totalH = buttonSize.y / 3 + 2; // cao = 1/3 chiều cao nút
			// căn giữa dọc:
			const float topY = pmin.y + (buttonSize.y - totalH) * 0.5f;
			const float bodyY0 = topY + stripeH;
			const float bodyY1 = topY + totalH - stripeH;

			// chia 3 seg ngang: 1px – (w‑2)px – 1px
			const float seg0 = 1.0f;
			const float seg2 = 1.0f;
			const float seg1 = w - seg0 - seg2;

			// màu gốc & hệ số sáng:
			ImVec4 base = ImVec4(0.0f, 0.4f, 0.75f, 1.0f);
			const float fct[5] = { 0.0f, 1.0f, 0.75f, 0.5f, 0.25f };
			// ma trận đầu và đuôi:
			const int head[2][3] = { { 4, 2, 4 }, { 3, 1, 3 } };
			const int tail[2][3] = { { 3, 1, 3 }, { 4, 2, 4 } };

			// — Vẽ đầu (2px) —
			for (int row = 0; row < 2; row++)
			{
				float y = topY + static_cast<float>(row);
				for (int col = 0; col < 3; col++)
				{
					float x0 = pmin.x + (col == 0 ? 0.0f : (col == 1 ? seg0 : seg0 + seg1));
					float wcol = (col == 0 ? seg0 : (col == 1 ? seg1 : seg2));
					float alpha = fct[head[row][col]];
					ImU32 colu = ImGui::GetColorU32(ImVec4(base.x, base.y, base.z, alpha));
					drawlist->AddRectFilled(ImVec2(x0, y), ImVec2(x0 + wcol, y + 1.0f), colu);
				}
			}

			// — Vẽ giữa (full màu) —
			{
				ImU32 colu = ImGui::GetColorU32(base);
				drawlist->AddRectFilled(ImVec2(pmin.x, bodyY0), ImVec2(pmin.x + w, bodyY1), colu);
			}

			// — Vẽ đuôi (2px) —
			for (int row = 0; row < 2; row++)
			{
				float y = bodyY1 + static_cast<float>(row);
				for (int col = 0; col < 3; col++)
				{
					float x0 = pmin.x + (col == 0 ? 0.0f : (col == 1 ? seg0 : seg0 + seg1));
					float wcol = (col == 0 ? seg0 : (col == 1 ? seg1 : seg2));
					float alpha = fct[tail[row][col]];
					ImU32 colu = ImGui::GetColorU32(ImVec4(base.x, base.y, base.z, alpha));
					drawlist->AddRectFilled(ImVec2(x0, y), ImVec2(x0 + wcol, y + 1.0f), colu);
				}
			}
		}

		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(); // pop ButtonTextAlign
		ImGui::PopStyleVar(); // pop FrameRounding
	}

	ImGui::End();
}

void giaodien_bangdl(const int chieurong_manhinh, const int chieucao_manhinh)
{
	thongtin_cuaso_imgui tt = tinh_thongtin_cuaso(chieurong_manhinh, chieucao_manhinh);

	ImGui::SetNextWindowPos(ImVec2(tt.vitri));
	ImGui::SetNextWindowSize(ImVec2(tt.kichthuoc));
	ImGui::Begin("bangdl", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	capnhat_bang_phanmem();
	ImGui::End();
}

void giaodien_tienich(const int chieurong_manhinh, const int chieucao_manhinh)
{
	thongtin_cuaso_imgui tt = tinh_thongtin_cuaso(chieurong_manhinh, chieucao_manhinh);

	ImGui::SetNextWindowPos(ImVec2(tt.vitri));
	ImGui::SetNextWindowSize(ImVec2(tt.kichthuoc));
	ImGui::Begin("tienich", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	hienthi_bangsuachua();
	ImGui::End();
}

void giaodien_caidat(const int chieurong_manhinh, const int chieucao_manhinh)
{
	const thongtin_cuaso_imgui tt = tinh_thongtin_cuaso(chieurong_manhinh, chieucao_manhinh);

	ImGui::SetNextWindowPos(tt.vitri);
	ImGui::SetNextWindowSize(tt.kichthuoc);
	ImGui::Begin("caidat", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	giaodien_tinhnang_xuatnap_cauhinh();

	ImGui::Separator();
	hienthi_nhapkey();

	ImGui::End();
}

void giaodien_hotro(const int chieurong_manhinh, const int chieucao_manhinh)
{
	thongtin_cuaso_imgui tt = tinh_thongtin_cuaso(chieurong_manhinh, chieucao_manhinh);

	ImGui::SetNextWindowPos(ImVec2(tt.vitri));
	ImGui::SetNextWindowSize(ImVec2(tt.kichthuoc));
	ImGui::Begin("hotro", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::Text("cảm ơn đã sử dụng sản phẩm");
	ImGui::Image(reinterpret_cast<ImTextureID>("imgid"), ImVec2(64, 64));
	ImGui::End();
}

void giaodien_tinhnang_xuatnap_cauhinh()
{
	if (ImGui::InputText("Đường dẫn thư mục", &dl.dd_xuat))
	{
		dl.loi_xuat_tepch.clear();
	}

	ImGui::BeginGroup();
	if (ImGui::Button("Xuất tệp"))
	{
		if (kiemtraduongdan_thumuc())
		{
			lp_xuat_cauhinh("cauhinh.ini");
		}
	}

	ImGui::SameLine();

	hienthi_loi(dl.loi_xuat_tepch, dl.thoigian_hienthi_loi_xuat);

	ImGui::EndGroup();
	ImGui::Separator();

	if (ImGui::InputText("Đường dẫn tệp tin", &dl.dd_nap))
	{
		dl.loi_nap_tepch.clear();
	}

	ImGui::BeginGroup();
	if (ImGui::Button("Nạp cấu hình"))
	{
		if (kiemtraduongdan_taptin())
		{
			ch.tep_nguon = dl.dd_nap;
			lp_chay_saochep_ini();
		}
	}

	ImGui::SameLine();

	hienthi_loi(dl.loi_nap_tepch, dl.thoigian_hienthi_loi_nap);

	ImGui::EndGroup();

	if (ImGui::Button("Mặc định"))
	{
		lp_nap_cauhinh_macdinh();
	}
}

void hienthi_bangsuachua()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));

	bool mo = ImGui::CollapsingHeader("🔧  Sửa chữa\nKhuyên dùng khi bị treo máy, lỗi xanh màn hình, cập nhật thất bại, v.v...",
									  ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_NoTreePushOnOpen);

	ImGui::PopStyleVar(2);

	if (mo)
	{
		ImGui::Spacing();

		ImGuiStyle& style = ImGui::GetStyle();
		float regionMaxX = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX();
		float iconSize = 20.0f;
		float topY = ImGui::GetCursorPosY();

		ImGui::SetCursorPosY(topY);
		ImGui::SameLine(regionMaxX - iconSize - style.FramePadding.x);
		ImGui::TextDisabled("?");

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImVec2 iconPos = ImGui::GetItemRectMin();
			float padding = 20.0f;

			// Nội dung tooltip
			const char* noidung = "DISM: kiểm tra và sửa chữa ảnh Windows cục bộ được dùng để thay thế các tệp hệ thống hoặc thành phần bị hỏng, "
								  "dựa trên bản sao sạch được tải về. (cần kết nối Internet)\n\n"
								  "SFC: quét tất cả các tệp hệ thống được bảo vệ và thay thế những tệp bị hỏng bằng bản sao đã được DISM cung cấp.\n\n"
								  "CHKDSK: kiểm tra và sửa chữa thiết bị lưu trữ của bạn (bad sector, lỗi đọc/ghi, lỗi hệ thống tệp). "
								  "Do phân vùng C đang được sử dụng bởi Windows, quá trình sẽ được lên lịch thực hiện sau khi khởi động lại.\n\n"
								  "Nếu không công cụ nào trong số này khắc phục được sự cố hiện tại, nên cài đặt lại Windows từ đầu.";

			float wrap_width = ImGui::GetFontSize() * 20.0f;
			ImVec2 size = ImGui::CalcTextSize(noidung, nullptr, true, wrap_width);
			ImVec2 tooltipPos(iconPos.x - size.x - padding, iconPos.y);

			ImGui::SetNextWindowPos(tooltipPos, ImGuiCond_Always);
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(wrap_width);
			ImGui::TextUnformatted(noidung);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		static bool chonDISM = false;
		static bool chonSFC = false;
		static bool chonCHKDSK = false;

		ImGui::SetCursorPosY(topY);
		ImGui::Indent(16.0f);
		if (!chonDISM && !chonSFC && !chonCHKDSK)
		{
			ImGui::TextColored(ImVec4(0.80f, 0.25f, 0.25f, 1.0f), "Vui lòng chọn ít nhất một công cụ");
		}
		else
		{
			ImVec2 size = ImGui::CalcTextSize("Vui lòng chọn ít nhất một công cụ");
			ImGui::Dummy(size);
		}

		ImGui::Checkbox("SFC   (Trình kiểm tra tập tin hệ thống)", &chonSFC);
		ImGui::Checkbox("DISM (Quản lý hình ảnh triển khai)", &chonDISM);
		ImGui::Checkbox("CHKDSK (Kiểm tra ổ đĩa)", &chonCHKDSK);

		ImGui::Unindent(16.0f);
		ImGui::Spacing();

		bool daChon = chonDISM || chonSFC || chonCHKDSK;
		ImGui::Indent(16.0f);
		ImGui::BeginDisabled(!daChon);
		if (ImGui::Button(" Sửa chữa", ImVec2(100, 0)))
			lp_suachua_nhieu(chonDISM, chonSFC, chonCHKDSK);

		ImGui::EndDisabled();
		ImGui::Unindent(16.0f);
	}
}

void hienthi_nhapkey()
{
	static char key_nhap[64] = "";
	static std::string ketqua;

	ImGui::InputText("Nhập key bản quyền", key_nhap, sizeof(key_nhap));

	if (ImGui::Button("Kiểm tra"))
	{
		auto kq_key = kiemtra_key_online(key_nhap);
		if (kq_key.has_value())
		{
			if (sosanh_ngay(kq_key->ngayhethan))
				ketqua = "hợp lệ";
			else
				ketqua = "hết hạn";
		}
		else
			ketqua = "không hợp lệ";
	}

	if (!ketqua.empty())
		ImGui::TextWrapped("%s", ketqua.c_str());
}

