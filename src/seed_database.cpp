#include "database.hpp"
#include "entities.hpp"
#include "utilities.hpp"

#include <cstring>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

static void to_ymd(int date, int &y, int &m, int &d) {
  y = date / 10000;
  m = (date / 100) % 100;
  d = date % 100;
}

static int from_ymd(int y, int m, int d) { return y * 10000 + m * 100 + d; }

static int add_days(int date, int delta) {
  std::tm t{};
  int y, m, d;
  to_ymd(date, y, m, d);

  t.tm_year = y - 1900;
  t.tm_mon = m - 1;
  t.tm_mday = d + delta;

  std::mktime(&t);

  return from_ymd(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
}

// ---------- Utilities ----------
static std::mt19937 rng(std::random_device{}());

static int rand_int(int min, int max) {
  std::uniform_int_distribution<int> dist(min, max);
  return dist(rng);
}

template <typename T, size_t N>
static const T &rand_from(const std::array<T, N> &arr) {
  return arr[rand_int(0, N - 1)];
}

static void copy_str(char *dest, const std::string &src, size_t maxSize) {
  memcpy(dest, src.c_str(), maxSize - 1);
  dest[maxSize - 1] = '\0';
}

// ---------- Dummy Data Pools ----------
static const std::vector<std::string> FIRST_NAMES = {
    "Aiman", "John",  "Nig",  "Ai",     "Rem", "Alya",
    "Li",    "Jason", "Zaza", "Johnny", "Rudy"};

static const std::vector<std::string> LAST_NAMES = {
    "Fo",     "Lee", "Tan",   "Lim", "Rahman", "Wong",
    "Ismail", "Ng",  "Kumar", "Ong", "Suzan",  "Greyrat"};

static const std::vector<std::string> VEH_MODELS = 
{"Myvi", "Civic", "Axia",  "Vios", "Saga",  "Subaru"};

// ---------- Generators ----------

static int generate_student_id() { return 2000000 + rand_int(0, 999999); }

static std::string generate_name() {
  return FIRST_NAMES[rand_int(0, FIRST_NAMES.size() - 1)] + " " +
         LAST_NAMES[rand_int(0, LAST_NAMES.size() - 1)];
}

static std::string generate_phone() {
  return "01" + std::to_string(rand_int(10000000, 99999999));
}

static std::string generate_plate() {
  std::string plate;

  for (int i = 0; i < 3; ++i)
    plate += char('A' + rand_int(0, 25));

  plate += std::to_string(rand_int(1000, 9999));

  return plate;
}

// ---------- Main Seeder ----------

void seed_database(int count = 100) {
  std::vector<int> userIds;
  std::vector<int> appIds;

  // ---- USERS ----
  for (int i = 0; i < count; ++i) {
    User u{};

    u.id = generate_student_id();
    copy_str(u.name, generate_name(), sizeof(u.name));
    copy_str(u.password, "pass123", sizeof(u.password));
    copy_str(u.contactNumber, generate_phone(), sizeof(u.contactNumber));
    copy_str(u.faculty, rand_from(C_FACULTIES), sizeof(u.faculty));

    u.age = rand_int(18, 30);
    u.status = eUserStatus::Active;
    u.role = eUserRole::Student;

    if (db_add_record(u)) {
      userIds.push_back(u.id);
    }
  }

  // ---- APPLICATIONS ----
  for (int i = 0; i < count; ++i) {
    if (userIds.empty())
      break;

    ParkingApplication app{};

    app.id = db_get_next_id<ParkingApplication>();
    app.userid = userIds[rand_int(0, userIds.size() - 1)];
    app.duration = rand_from(C_PASS_DURATION);

    int today = date::current();

    // submission within last 90 days
    app.submissionDate = add_days(today, -rand_int(0, 90));

    if (rand_int(0, 1)) {
      // CLOSED
      app.closedDate = add_days(app.submissionDate, rand_int(1, 30));
      app.status = static_cast<eApplicationStatus>(
          rand_int(0, 1)); // Completed / Rejected
    } else {
      // STILL ACTIVE
      app.closedDate = 0;
      app.status = static_cast<eApplicationStatus>(
          rand_int(2, 3)); // PendingPayment / WaitingForReview
    }

    if (db_add_record(app)) {
      appIds.push_back(app.id);
    }
  }

  // ---- PASSES ----
  for (int i = 0; i < count; ++i) {
    if (userIds.empty() || appIds.empty())
      break;

    ParkingPass pass{};

    int appId = appIds[rand_int(0, appIds.size() - 1)];
    pass.id = db_get_next_id<ParkingPass>();

    ParkingApplication app{};
    db_find_by_id(appId, &app);

    pass.userid = app.userid;
    pass.appid = app.id;

    pass.duration = rand_from(C_PASS_DURATION);
    int today = date::current();

    // issue date within last 120 days
    pass.issueDate = add_days(today, -rand_int(0, 120));

    // compute expiry
    int expiryDate = add_days(pass.issueDate, pass.duration);

    // derive status from real validity
    if (expiryDate < today) {
      pass.status = ePassStatus::Expired;
    } else {
      pass.status = ePassStatus::Active;
    }
    db_add_record(pass);
  }

  // ---- VEHICLES ----
  for (int i = 0; i < count; ++i) {
    if (userIds.empty())
      break;

    Vehicle v{};

    v.id = db_get_next_id<Vehicle>();
    v.userid = userIds[rand_int(0, userIds.size() - 1)];

    // model
    copy_str(v.model, VEH_MODELS[rand_int(0, VEH_MODELS.size() - 1)],
             sizeof(v.model));

    std::unordered_set<std::string> usedPlates;
    // unique-ish plate
    std::string plate;
    do {
      plate = generate_plate();
    } while (usedPlates.count(plate));

    usedPlates.insert(plate);
    copy_str(v.plate, plate, sizeof(v.plate));

    db_add_record(v);
  }
}