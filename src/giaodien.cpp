//giaodien.cpp
#include "chay_luongphu.h"
#include "chucnang_cotloi.h"
#include "giaodien.h"
#include "logic_giaodien.h"

#include <imgui_stdlib.h>


giaodien gd;

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

// Hàm hỗ trợ để tự động sửa đổi màu
ImVec4 adjust_color_brightness(const ImVec4& color, const float factor)
{
	return {
		std::min(color.x * factor, 1.0f),
		std::min(color.y * factor, 1.0f),
		std::min(color.z * factor, 1.0f),
		color.w // Giữ nguyên độ trong suốt (alpha)
	};
}

void combo_box(const char* nhãn, const char* options[], const int options_count, int& current_selection, const float gt_botron)
{
	// Nếu chưa chọn gì (currentSelection == 0), hiển thị chuỗi rỗng ""
	const char* preview = (current_selection == 0) ? "" : options[current_selection];

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, gt_botron);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Giữ nguyên màu nền
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Không đổi màu khi di chuột vào
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Đen nhưng hoàn toàn trong suốt
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Đặt màu nền danh sách hạ xuống

	if (ImGui::BeginCombo(nhãn, preview, ImGuiComboFlags_NoArrowButton))
	{
		for (int i = 1; i < options_count; ++i) // Bỏ qua option đầu tiên (rỗng)
		{
			if (i != current_selection && ImGui::Selectable(options[i])) // Bỏ qua cái đã chọn
				current_selection = i;
		}
		ImGui::EndCombo();
	}

	ImGui::PopStyleColor(7);
	ImGui::PopStyleVar();

	ImGui::SameLine();
	const std::string btn_label = std::string("X##") + nhãn;

	if (ImGui::Button(btn_label.c_str()))
		current_selection = 0; // Reset về trạng thái không hiển thị gì
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
		//hiển thị dữ liệu cột
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
				} else
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
						//ImGui::Text("%s", lg_gd.data[hang][vitri_dulieu].c_str());
						ImGui::TextUnformatted(lg_gd.data[hang][vitri_dulieu].c_str());
					}
				}
				if (cot.id != "chon")
				{
					float columnWidth = ImGui::GetColumnWidth();
					ImGui::SetCursorScreenPos(cellPos);

					if (ImGui::InvisibleButton(
						("##overlay_" + std::to_string(hang) + "_" + std::to_string(vitri_cot)).c_str(),
						ImVec2(columnWidth, rowHeight)))
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

		//ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		//// Màu khung xanh lá, độ dày 2.0f
		//draw_list->AddRect(menuPos, ImVec2(menuPos.x + menuSize.x, menuPos.y + menuSize.y),
		//				   IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);

		// Kiểm tra click chuột trái bên ngoài vùng menu để đóng menu
		if (ImGui::IsMouseClicked(0))
		{
			if (!ImGui::IsMouseHoveringRect(toggleButtonMin, toggleButtonMax))
			{
				ImVec2 mousePos = ImGui::GetIO().MousePos;
				if (mousePos.x < menuPos.x || mousePos.x > menuPos.x + menuSize.x ||
					mousePos.y < menuPos.y || mousePos.y > menuPos.y + menuSize.y)
				{
					gd.isMenuOpen = false;
				}
			}
		}
	}
}

void giaodien_menuben(const int chieucao_manhinh)
{
	gd.chieucao_menuben = static_cast<float>(chieucao_manhinh) - gd.letren_menuben;

	// Xử lý thu gọn menu
	const float chieurong_hientai = tt_thugonkichthuoc(gd.menuben_thugon, gd.yeucau_thugon, gd.batdau_thugon,
													   gd.chieurong_menuben_morong, gd.chieurong_menuben_thugon,
													   gd.thoigian_thugon);

	gd.chieurong_menuben = chieurong_hientai;

	ImGui::SetNextWindowPos(ImVec2(gd.letrai_menuben, gd.letren_menuben));
	ImGui::SetNextWindowSize(ImVec2(chieurong_hientai, gd.chieucao_menuben));
	ImGui::Begin("Menu bên", nullptr,
				 ImGuiWindowFlags_NoMove |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoScrollbar |
				 ImGuiWindowFlags_NoScrollWithMouse);

	auto mau_nut = ImVec4(0, 0, 0, 0);
	ImVec4 mau_nut_hover = (mau_nut.w == 0.0f) ? ImVec4(0.3f, 0.3f, 0.3f, 0.2f) : adjust_color_brightness(mau_nut, 0.8f);
	ImVec4 mau_nut_active = (mau_nut.w == 0.0f) ? ImVec4(0.2f, 0.2f, 0.2f, 0.4f) : adjust_color_brightness(mau_nut, 1.2f);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, mau_nut);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, mau_nut_hover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, mau_nut_active);

	constexpr float chieucao_nut = 30.0f;
	const ImVec2 kichthuoc_nut(chieurong_hientai, chieucao_nut);

	const std::vector<MenuItem> menu_items = {
		{L"Bảng dữ liệu", "bangdl"},
		{L"Tiện ích", "tienich"},
		{L"Cài đặt", "caidat"},
	};

	for (const auto& [full_text, id] : menu_items)
	{
		ImVec2 vitri_nut = ImGui::GetCursorScreenPos();
		vitri_nut.x = gd.letrai_menuben;
		ImGui::SetCursorScreenPos(vitri_nut);

		// Tính toán tỷ lệ để thu gọn chữ
		std::string vanban_nhinthay = tt_vanbancothenhinthay(full_text,
															 chieurong_hientai,
															 gd.chieurong_menuben_thugon,
															 gd.chieurong_menuben_morong);

		ImVec2 vitri_vanban = ImGui::GetCursorScreenPos();
		vitri_vanban.x += 10.0f;
		vitri_vanban.y += 10.0f;

		if (ImGui::Button(("###" + id).c_str(), kichthuoc_nut))
		{
			if (id == "bangdl") ImGui::SetWindowFocus("bangdl");
			if (id == "tienich") ImGui::SetWindowFocus("tienich");
			if (id == "caidat") ImGui::SetWindowFocus("caidat");
		}

		ImGui::GetWindowDrawList()->AddText(vitri_vanban, ImGui::GetColorU32(ImGuiCol_Text), vanban_nhinthay.c_str());
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	ImVec2 vitri_nut;
	vitri_nut.x = gd.letrai_menuben;
	vitri_nut.y = gd.chieucao_menuben + chieucao_nut * 3;
	ImGui::SetCursorScreenPos(vitri_nut);

	constexpr auto button_thugon_color = ImVec4(0.6f, 0.6f, 0.6f, 0.2f);
	const ImVec4 button_thugon_hover_color = adjust_color_brightness(button_thugon_color, 0.8f);
	const ImVec4 button_thugon_active_color = adjust_color_brightness(button_thugon_color, 1.2f);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, button_thugon_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_thugon_hover_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_thugon_active_color);

	if (ImGui::Button(gd.menuben_thugon ? ">" : "<", kichthuoc_nut))
	{
		gd.yeucau_thugon = true;
		gd.batdau_thugon = std::chrono::steady_clock::now();
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

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

	if (ImGui::Button("Sửa Window"))
		logic_giaodien::chaylenh_tienich();

	ImGui::End();
}

void giaodien_caidat(const int chieurong_manhinh, const int chieucao_manhinh)
{
	const thongtin_cuaso_imgui tt = tinh_thongtin_cuaso(chieurong_manhinh, chieucao_manhinh);

	ImGui::SetNextWindowPos(tt.vitri);
	ImGui::SetNextWindowSize(tt.kichthuoc);
	ImGui::Begin("caidat", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	giaodien_tinhnang_xuatnap_cauhinh();
	/*const char* options[] = { "Option 1", "Option 2", "Option 3" };
	int current_selection = 0;
	combo_box("a", options, 3, current_selection, 4.0f);*/
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


