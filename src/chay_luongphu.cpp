// chay_luongphu.cpp
#include "chay_luongphu.h"
#include "chucnang_cotloi.h"
#include "csdl.h"
#include "logic_giaodien.h"
#include "thread_pool.h"

std::future<void> lp_chay_capnhat()
{
	csdl c;
	return pool.enqueue(capnhat_data, c);
}

std::future<void> lq_khoidong_sql()
{
	return pool.enqueue(khoidong_sql);
}

void lp_chay_lenhwinget(const std::string& id)
{
	pool.enqueue(chaylenh, id);
}

void lp_suachua_nhieu(bool dism, bool sfc, bool chk)
{
	pool.enqueue(
			[dism, sfc, chk]
			{
				std::vector<std::string> cmds;
				if (dism)
					cmds.emplace_back("dism /Online /Cleanup-Image /RestoreHealth");

				if (sfc)
					cmds.emplace_back("sfc /scannow");

				if (chk)
				{
					const auto cmds_chkdsk = tao_lenh_chkdsk();
					cmds.insert(cmds.end(), cmds_chkdsk.begin(), cmds_chkdsk.end());
				}

				if (!cmds.empty())
					suachua_nhieu(cmds);
			});
}

void lp_mo_phanmanh()
{
	pool.enqueue([] { mo_phanmanh(); });
}

void lp_chay_phanmanh()
{
	pool.enqueue([] { chay_phanmanh(); });
}

void lp_chay_xoarac()
{
	pool.enqueue([] { tienhanh_xoa(); });
}

void lp_nap_cauhinh()
{
	pool.enqueue(nap_cauhinh);
}

void lp_xuat_cauhinh(const std::string& dd)
{
	pool.enqueue(xuat_cauhinh, dd);
}

void lp_nap_cauhinh_macdinh()
{
	pool.enqueue(ch_macdinh);
}

void lp_chay_saochep_ini()
{
	pool.enqueue(saochep_ini);
}

std::future<std::optional<ketqua_key>> lq_kiemtra_key_async(const std::string& key)
{
	return pool.enqueue(kiemtra_key_online, key);
}
