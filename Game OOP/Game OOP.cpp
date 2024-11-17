#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
#include<string>
#include <random>
#include <unordered_map>
#include<vector>

using namespace std;

int player_score = 0;
int cpu_score = 0;

class SoundManager {
public:
    static SoundManager& getInstance() {
        static SoundManager instance;
        return instance;
    }
    // Hàm để tải âm thanh từ file
    bool loadSound(const std::string& key, const std::string& filename) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) {
            // Nếu không thể tải âm thanh, trả về false
            cout << "\nFail to load Sound";
            return false;
        }
        // Thêm buffer vào map với key tương ứng
        soundBuffers[key] = buffer;
        return true;
    }

    // Hàm để phát âm thanh
    void playSound(const std::string& key) {
        if (soundBuffers.find(key) != soundBuffers.end() &&isMuted==false) {
            sf::Sound sound;
            sound.setBuffer(soundBuffers[key]);
            sounds.push_back(sound);
            sounds.back().play();
        }
    }
    void Mute() {
        isMuted = !isMuted;
    }
    bool isMute() {
        return isMuted;
    }  
private:
    // Map lưu trữ các buffer âm thanh với key
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    // List lưu trữ các đối tượng âm thanh đang phát
    std::vector<sf::Sound> sounds;
    SoundManager() {}  // Private constructor to enforce singleton pattern
    bool isMuted ;
};

class BlinkingEffect
{
public:
    static BlinkingEffect& getInstance()
    {
        static BlinkingEffect instance;
        return instance;
    }
    void start()
    {
        clock.restart();
        isFlashing = true;
    }
    void setParameters(float duration, const sf::Color& color)
    {
        flashDuration = duration;
        flashColor = color;
    }
    void update(sf::Text& text)
    {
        if (isFlashing)
        {   //check lại cái này
            float elapsedTime = clock.getElapsedTime().asSeconds();
            if (elapsedTime < flashDuration)
            {
                // Thay đổi màu sắc để tạo hiệu ứng nhấp nháy
                text.setFillColor(flashColor);
            }
            else
            {
                // Trở lại màu sắc bình thường sau khi hiệu ứng nhấp nháy kết thúc
                text.setFillColor(sf::Color::White);
                isFlashing = false;
            }
        }
    }
private:
    BlinkingEffect() : flashDuration(0.5f), flashColor(sf::Color::Yellow), isFlashing(false) {}

    sf::Clock clock;
    float flashDuration;
    sf::Color flashColor;
    bool isFlashing;
};

int getRandomValue() {
    std::random_device rd;   // Thu thập seed ngẫu nhiên
    std::mt19937 gen(rd());  // Sinh số ngẫu nhiên Mersenne Twister với seed

    // Tạo phạm vi chỉ có 2 giá trị: 0 và 1
    std::uniform_int_distribution<> distrib(0, 1);

    // Sinh ra 1 hoặc -1
    return distrib(gen) == 0 ? -1 : 1;
}
class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw(sf::RenderWindow& window) {
        sf::CircleShape circle(radius);
        circle.setFillColor(sf::Color (243, 213, 91, 255));
        circle.setPosition(x - radius, y - radius);
        window.draw(circle);
    }

    void Update(sf::RenderWindow& window) {
        x += speed_x;
        y += speed_y;
        if (y >= window.getSize().y - radius || y <= radius) {
            speed_y *= -1;
        }
        if (x >= window.getSize().x - radius) {
            cpu_score++;
            SoundManager::getInstance().playSound("lose");
            ResetBall(window);
        } 
        if (x <= radius) {
            player_score++;
            SoundManager::getInstance().playSound("point");
            BlinkingEffect::getInstance().start();
            ResetBall(window);
        }
    }
    void ResetBall(sf::RenderWindow& window) {
        x = window.getSize().x / 2;
        y = window.getSize().y / 2;
        speed_x *= -1;
        speed_y *= getRandomValue();
    }
};

class PauseManager
{
public:
    PauseManager()
    {
        // Tạo nút pause
        pauseButton.setSize(sf::Vector2f(120, 60)); // Kích thước lớn hơn cho dễ bấm
        pauseButton.setPosition(1140, 20); // Vị trí đã được điều chỉnh để tránh rìa màn hình
        pauseButton.setFillColor(sf::Color::Black); // Nền màu đen
        pauseButton.setOutlineThickness(3); // Độ dày viền
        pauseButton.setOutlineColor(sf::Color::White); // Màu viền trắng

        // Tạo text "Pause"
        if (!font.loadFromFile("Gameplay.ttf"))
        {
            // Xử lý lỗi nếu không tải được font
        }
        pauseText.setFont(font);
        pauseText.setString("Pause");
        pauseText.setCharacterSize(24);
        pauseText.setFillColor(sf::Color::White);

        // Căn chỉnh văn bản để nằm giữa nút
        sf::FloatRect textRect = pauseText.getLocalBounds();
        pauseText.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        pauseText.setPosition(pauseButton.getPosition() + sf::Vector2f(pauseButton.getSize().x / 2.0f,
            pauseButton.getSize().y / 2.0f));
    }

    void handleEvent(const sf::RenderWindow& window, const sf::Event& event)
    {  
        if (event.type == sf::Event::MouseButtonPressed && isPaused==false) //Kiem tra bam
        {
            if (pauseButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) //Kiem tra con tro co nam trong vi tri cua nut khong
            {
                isPaused = !isPaused; // Đảo ngược trạng thái pause
                if (isPaused){
                    pauseButton.setFillColor(sf::Color::Red); // Đổi màu nút khi pause
            }
                else
                    pauseButton.setFillColor(sf::Color::Black);   // Đổi màu lại khi resume
            }
        }
        if (isPaused) {
            pauseButton.setFillColor(sf::Color::Red); // Đổi màu nút khi pause
        }
        else
            pauseButton.setFillColor(sf::Color::Black);
    }
    void setPaused(bool paused) {
        isPaused = paused;
    }
    bool isGamePaused() const
    {
        return isPaused;
    }

    void drawPause(sf::RenderWindow& window)
    {
        window.draw(pauseButton);
        window.draw(pauseText);
    }

private:
    sf::RectangleShape pauseButton;
    sf::Text pauseText;
    sf::Font font;
    bool isPaused = false;
};

class Paddle {
protected:
    void LimitMovement(sf::RenderWindow& window) {
        if (y <= 5) {
            y = 5;
        }
        if (y + height + 5 >= window.getSize().y) {
            y = window.getSize().y - height-5;
        }
    }
public:
    float x, y;
    float width, height;
    int speed;
    void Draw(sf::RenderWindow& window) {
        sf::RectangleShape rectangle2(sf::Vector2f(width, height));
        rectangle2.setFillColor(sf::Color::White);
        rectangle2.setPosition(x, y);
        window.draw(rectangle2);
    }
    void Update(sf::RenderWindow& window) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && y > 0) {
            y -= speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && y < window.getSize().y - height) {
            y += speed;
        }
        LimitMovement(window);
    }
    void Collision(Ball& ball) {
        if (y <= ball.y && ball.y <= y + height && ball.x + ball.radius >= x) {
            ball.speed_x *= -1;
            SoundManager::getInstance().playSound("hit");
        }
    }
    void resetPosition(sf::RenderWindow& window) {
        x = window.getSize().x - width - 10;
        y = window.getSize().y / 2 - height / 2;       
    }
};

//Ke thua
class CpuPaddle : public Paddle {
public:
    void Update(int ball_y, sf::RenderWindow& window) {
        if (y + height / 2 > ball_y) {
            y -= speed;
        }
        if (y + height / 2 < ball_y) {
            y += speed;
        }
        LimitMovement(window);
    }
    void Collision(Ball& ball) {
        if (y <= ball.y && ball.y <= y + height && ball.x-ball.radius<= x+width) {
            ball.speed_x *= -1;
            SoundManager::getInstance().playSound("hit");
        }
    }
    void resetPosition(sf::RenderWindow& window) {
        x = 10;
        y = window.getSize().y / 2 - height / 2;
    }
};


class PausePopup {
public:
    
    PausePopup(const sf::Font& font, PauseManager& pm, Ball& ball, Paddle& playerPaddle, CpuPaddle& aiPaddle, int& player_score, int& cpu_score) : pauseManager(pm), ball(ball), playerPaddle(playerPaddle), aiPaddle(aiPaddle), player_score(player_score), cpu_score(cpu_score)
    {
        // Màn hình mờ
        overlay.setSize(sf::Vector2f(1280, 800));
        overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Màu đen mờ

        // Tạo popup
        popup.setSize(sf::Vector2f(400, 300)); // Kích thước popup
        popup.setFillColor(sf::Color::White); // Màu nền popup
        popup.setOutlineThickness(5); // Độ dày viền
        popup.setOutlineColor(sf::Color(100, 100, 100)); // Màu viền
        popup.setPosition(640 - popup.getSize().x / 2, 400 - popup.getSize().y / 2);

        // Tạo bóng đổ cho popup
        shadow.setSize(sf::Vector2f(popup.getSize().x + 10, popup.getSize().y + 10));
        shadow.setFillColor(sf::Color(0, 0, 0, 100)); // Màu đen mờ cho bóng đổ
        shadow.setPosition(popup.getPosition().x + 5, popup.getPosition().y + 5); // Vị trí bóng đổ

        // Tạo văn bản tiêu đề
        title.setFont(font);
        title.setString("My Pong Game");
        title.setCharacterSize(30);
        title.setFillColor(sf::Color::Black);
        title.setPosition(popup.getPosition().x + popup.getSize().x / 2 - title.getGlobalBounds().width / 2,
            popup.getPosition().y + 20);

        // Tạo các nút
        createButton(resumeButton, sf::Color::Green, sf::Vector2f(150, 40), "Resume", font, { 0, -60 });
        createButton(restartButton, sf::Color::Blue, sf::Vector2f(150, 40), "Restart", font, { 0, 0 });
        createButton(muteButton, sf::Color::Red, sf::Vector2f(150, 40), "Mute Music", font, { 0, 60 });
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(overlay); // Vẽ màn hình mờ
        window.draw(shadow);  // Vẽ bóng đổ
        window.draw(popup);   // Vẽ popup
        window.draw(title);   // Vẽ tiêu đề
        window.draw(resumeButton); // Vẽ nút Resume
        window.draw(restartButton); // Vẽ nút Restart
        window.draw(muteButton); // Vẽ nút Mute
        window.draw(resumeText); // Vẽ văn bản Resume
        window.draw(restartText); // Vẽ văn bản Restart
        window.draw(muteText); // Vẽ văn bản Mute Music
    }
    void restart(sf::RenderWindow& window) {
        // Đặt lại vị trí và tốc độ ban đầu cho bóng
        ball.ResetBall(window);  // Đặt bóng ở giữa màn hình

        // Đặt lại vị trí cho thanh trượt
        playerPaddle.resetPosition(window);  // Đặt thanh trượt của người chơi
        aiPaddle.resetPosition(window);  // Đặt thanh trượt của AI

        // Đặt lại điểm số
        player_score = 0;
        cpu_score = 0;

        // Đặt lại trạng thái trò chơi (nếu cần)
        pauseManager.setPaused(false);  // Bỏ pause nếu trò chơi đang bị dừng
    }
    void handleEvent( sf::RenderWindow& window,const sf::Event& event )
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (resumeButton.getGlobalBounds().contains(window.mapPixelToCoords(mousePos)))
            {
                // Xử lý nhấn nút Resume
                pauseManager.setPaused(false);
            }
            else if (restartButton.getGlobalBounds().contains(window.mapPixelToCoords(mousePos)))
            {
                // Xử lý nhấn nút Restart
                restart(window);
            }
            else if (muteButton.getGlobalBounds().contains(window.mapPixelToCoords(mousePos)))
            {
                // Xử lý nhấn nút Mute Music
                SoundManager::getInstance().Mute();
            }
        }
    }
private:
    void createButton(sf::RectangleShape& button, const sf::Color& color, const sf::Vector2f& size, const std::string& text, const sf::Font& font, const sf::Vector2f& offset)
    {
        button.setSize(size);
        button.setFillColor(color);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color::Black);
        button.setPosition(popup.getPosition().x + popup.getSize().x / 2 - size.x / 2,
            popup.getPosition().y + popup.getSize().y / 2 - size.y / 2 + offset.y);

        sf::Text buttonText;
        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(20);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(button.getPosition().x + size.x / 2 - buttonText.getGlobalBounds().width / 2,
            button.getPosition().y + size.y / 2 - buttonText.getGlobalBounds().height / 2);

        if (text == "Resume")
            resumeText = buttonText;
        else if (text == "Restart")
            restartText = buttonText;
        else if (text == "Mute Music")
            muteText = buttonText;
    }
    sf::RectangleShape overlay;
    sf::RectangleShape popup;
    sf::RectangleShape shadow;
    sf::RectangleShape resumeButton;
    sf::RectangleShape restartButton;
    sf::RectangleShape muteButton;
    sf::Text title;
    sf::Text resumeText;
    sf::Text restartText;
    sf::Text muteText;
    PauseManager& pauseManager;
    Ball& ball;
    Paddle& playerPaddle;
    CpuPaddle& aiPaddle;
    int& player_score;
    int& cpu_score;
};


Ball ball;
Paddle player;
CpuPaddle cpu;

int main()
{
    cout << "Starting the game" << endl;
    const int screen_width = 1280;
    const int screen_height = 800;
    sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "My Pong Game!");
    window.setFramerateLimit(60);
    SoundManager::getInstance().loadSound("hit", "hit_sound.wav");
    SoundManager::getInstance().loadSound("point", "point.wav");
    SoundManager::getInstance().loadSound("lose", "lose.wav");


    PauseManager pauseManager;

    BlinkingEffect& blinkingEffect = BlinkingEffect::getInstance();
    blinkingEffect.setParameters(0.5f, sf::Color::Yellow);

        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile("sound.wav")) // Đảm bảo bạn có file sound.ogg trong thư mục
        {
            std::cout << "Could not load sound file!" << std::endl;
            return -1;
        }
        // Tạo đối tượng sf::Sound và gán buffer
        sf::Sound sound;
        sound.setBuffer(buffer);
        // Đặt âm thanh lặp đi lặp lại
        sound.setLoop(true);
        // Phát âm thanh
        sound.setVolume(30);
        if (!SoundManager::getInstance().isMute()) {
            sound.play();
        }


    sf::Color grassGreen(20, 160, 133, 255);
    sf::Color playerSide(38, 185, 154, 255);
    sf::RectangleShape rightHalf(sf::Vector2f(window.getSize().x/2, window.getSize().y ));
    rightHalf.setFillColor(playerSide);
    rightHalf.setPosition(window.getSize().x / 2, 0);

    sf::Color circleColor(129, 204, 184, 255);
    sf::CircleShape circle(80);
    circle.setFillColor(circleColor);
    circle.setPosition(
        (window.getSize().x - circle.getRadius() * 2) / 2,
        (window.getSize().y - circle.getRadius() * 2) / 2
    );

    //Gan gia tri thuoc tinh cua bong
    ball.radius = 20;
    ball.speed_x = 7;
    ball.speed_y = 7;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;

    //Gan gia tri thuoc tinh nguoi choi
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height/2 - player.height / 2;
    player.speed = 6;

    //Gan gia tri cho may
    cpu.width = 25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y= screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = sf::Vector2f(screen_width/2, 0);
    line[0].color = sf::Color::White;
    line[1].position = sf::Vector2f(screen_width/2, screen_height);
    line[1].color = sf::Color::White;
    //Font score
    sf::Font font;
    if (!font.loadFromFile("Gameplay.ttf")) {
        cout << "Cannot load font";
        return -1;
    }

    PausePopup pausePopup(font, pauseManager,ball,player,cpu,player_score,cpu_score);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(50);  // Kích thước chữ
    scoreText.setFillColor(sf::Color::White);  // Màu sắc chữ
    scoreText.setPosition(screen_width / 4, 20);  // Vị trí hiển thị tỷ số

    sf::Text scoreText1;
    scoreText1.setFont(font);
    scoreText1.setCharacterSize(50);  // Kích thước chữ
    scoreText1.setFillColor(sf::Color::White);  // Màu sắc chữ
    scoreText1.setPosition(3*screen_width / 4, 20);  // Vị trí hiển thị tỷ số

    while (window.isOpen())
    { 
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            pauseManager.handleEvent(window, event);
            pausePopup.handleEvent(window, event);
        }

        //Tô nền trước
        window.clear(grassGreen); //Xoa man hinh de chuan bi ve moi
        window.draw(rightHalf);

        window.draw(circle);
        if (!pauseManager.isGamePaused())
        {
            // Update game logic (di chuyển đối tượng, xử lý va chạm, vv...)
            //Ve doi tuong cpu va xu li logic
            cpu.Update(ball.y, window);
            cpu.Collision(ball);

            //Ve doi tuong nguoi choi va xu li logic
            player.Update(window);
            player.Collision(ball);

            //Ve doi tuong qua bong
            ball.Update(window);

            blinkingEffect.update(scoreText1);
            //Ve bang ti so
            scoreText.setString(to_string(cpu_score));
            scoreText1.setString(to_string(player_score));
        }
        cpu.Draw(window);
        player.Draw(window);
        ball.Draw(window);
        window.draw(line);
        window.draw(scoreText);
        window.draw(scoreText1);
        if(pauseManager.isGamePaused()) {
            sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
            overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Làm mờ màn hình

            pausePopup.draw(window);
       
        }

        
        pauseManager.drawPause(window);

        //Cap nhat va hien thi noi dung
        window.display(); 
    }
    window.close();
    return 0;
}